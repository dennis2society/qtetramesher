/*
 * QGLTetraMesh.cpp
 *
 *  Created on: Sep 11, 2011
 *      Author: Dennis Luebke
 */

#include "QGLTetraMesh.hpp"

#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>
#include <QtmGLViewer/Camera.hpp>
#include <QtmGLViewer/MeshData.hpp>

#include "TetraMeshTools/GMSHMeshLoader.h"
#include "TetraMeshTools/GMSHMeshWriter.h"
#include "TetraMeshTools/TriMeshLoader.h"
#include "TetraMeshTools/TriMeshWriter.h"
#include "TetgenWrapper.h"
#include "Timer.h"

// The mesh containers are uploaded to the GPU without conversion.
static_assert(sizeof(Vec3f) == 3 * sizeof(float), "Vec3f must be 3 floats");
static_assert(sizeof(Edge) == 2 * sizeof(uint32_t), "Edge must be 2 indices");
static_assert(sizeof(Triangle) == 3 * sizeof(uint32_t),
              "Triangle must be 3 indices");

namespace {

/// Instanced tetrahedron shader: every instance is one tetrahedron given by
/// its 4 corners, drawn as 12 vertices (4 faces) shrunk towards the
/// centroid. Tetrahedra whose centroid is outside the cutplane range are
/// collapsed outside the clip volume.
const char *kTetraVertexShader = R"(#version 330 core
layout(location = 0) in vec3 a_v0;
layout(location = 1) in vec3 a_v1;
layout(location = 2) in vec3 a_v2;
layout(location = 3) in vec3 a_v3;
uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;
uniform mat3 u_normalMatrix;
uniform vec3 u_cutMin;
uniform vec3 u_cutMax;
uniform float u_shrink;
out vec3 v_normal;
// faces as in the original immediate mode code, and the vertex opposite to
// each face
const int faces[12] = int[12](0, 3, 2, 3, 1, 2, 1, 0, 2, 0, 1, 3);
const int opposite[4] = int[4](1, 0, 3, 2);
void main() {
  vec3 v[4] = vec3[4](a_v0, a_v1, a_v2, a_v3);
  vec3 center = 0.25 * (v[0] + v[1] + v[2] + v[3]);
  if (any(lessThan(center, u_cutMin)) ||
      any(greaterThan(center, u_cutMax))) {
    v_normal = vec3(0.0, 0.0, 1.0);
    gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
    return;
  }
  int face = gl_VertexID / 3;
  int corner = gl_VertexID % 3;
  vec3 a = v[faces[face * 3]];
  vec3 b = v[faces[face * 3 + 1]];
  vec3 c = v[faces[face * 3 + 2]];
  vec3 n = cross(b - a, c - a);
  // make the face point outwards regardless of the tetra orientation
  if (dot(n, v[opposite[face]] - a) > 0.0) {
    n = -n;
    if (corner == 1) corner = 2;
    else if (corner == 2) corner = 1;
  }
  vec3 p = corner == 0 ? a : (corner == 1 ? b : c);
  p = center + (p - center) * u_shrink;
  v_normal = u_normalMatrix * normalize(n);
  gl_Position = u_projection * u_view * u_model * vec4(p, 1.0);
}
)";

const char *kTetraFragmentShader = R"(
in vec3 v_normal;
uniform vec3 u_color;
out vec4 fragColor;
void main() {
  vec3 n = gl_FrontFacing ? v_normal : -v_normal;
  fragColor = vec4(qtmHeadlight(n, u_color, 0.6, 50.0), 1.0);
}
)";

QColor toQColor(const Vec3f &c) { return QColor::fromRgbF(c.x, c.y, c.z); }

QVector3D toQVector(const Vec3f &v) { return QVector3D(v.x, v.y, v.z); }

const float *toFloats(const std::vector<Vec3f> &v) { return &v[0].x; }

/// true if all tetrahedron indices refer to existing vertices. Invalid
/// indices (e.g. from a corrupt file) would crash the topology generation
/// and the GPU upload.
bool TetraIndicesValid(const std::vector<Vec3f> &verts,
                       const std::vector<Tetrahedron> &tetras) {
  for (const Tetrahedron &t : tetras) {
    for (int k = 0; k < 4; ++k) {
      if (t.index[k] >= verts.size()) return false;
    }
  }
  return true;
}

}  // namespace

QGLTetraMesh::QGLTetraMesh()
    : drawSolid(false),
      drawTetraMesh(false),
      drawTetraSurface(false),
      drawTriangleMesh(true),
      isReady(false),
      showBoundingBox(false),
      bboxMode(0),
      drawTetraSolid(true),
      drawTetraWireframe(true),
      surfaceMode(0),
      octreeMode(0),
      xLowerThreshold(0.0f),
      xUpperThreshold(1.0f),
      yLowerThreshold(0.0f),
      yUpperThreshold(1.0f),
      zLowerThreshold(0.0f),
      zUpperThreshold(1.0f),
      surfaceDirty(true),
      tetraDirty(true),
      boundingVolumeDirty(true),
      octreeDirty(true),
      boundingVolumeBufferMode(-1),
      octreeBufferMode(-1) {
  top = NULL;
  surf = NULL;
  surfaceColorSolid = Vec3f(0.3f, 0.3f, 0.3f);
  tetraColorSolid = Vec3f(0.5f, 0.5f, 0.1f);
  surfaceColorWireframe = Vec3f(0.1f, 0.6f, 0.1f);
  tetraColorWireframe = Vec3f(0.2f, 0.2f, 0.6f);
  oct = NULL;
}

QGLTetraMesh::~QGLTetraMesh() {
  isReady = false;
  ClearAll();
}

void QGLTetraMesh::ClearAll() {
  // the octree references the surface mesh, so it has to go first
  delete oct;
  oct = NULL;
  delete top;
  top = NULL;
  delete surf;
  surf = NULL;
  MarkGeometryDirty();
}

void QGLTetraMesh::InitGL() {
  const QString fragment = QStringLiteral("#version 330 core\n") +
                           QString::fromLatin1(qtm::RenderContext::lightingGlsl()) +
                           QString::fromLatin1(kTetraFragmentShader);
  tetraProgram = std::make_unique<QOpenGLShaderProgram>();
  if (!tetraProgram->addShaderFromSourceCode(QOpenGLShader::Vertex,
                                             kTetraVertexShader) ||
      !tetraProgram->addShaderFromSourceCode(QOpenGLShader::Fragment,
                                             fragment) ||
      !tetraProgram->link()) {
    std::cerr << "Error building tetrahedron shader: "
              << tetraProgram->log().toStdString() << std::endl;
    tetraProgram.reset();
  }
  MarkGeometryDirty();
}

void QGLTetraMesh::CleanupGL() {
  surfaceSolidBuffer.destroy();
  surfaceWireframeBuffer.destroy();
  tetraSolidBuffer.destroy();
  tetraWireframeBuffer.destroy();
  boundingVolumeBuffer.destroy();
  octreeBuffer.destroy();
  tetraProgram.reset();
  MarkGeometryDirty();
}

void QGLTetraMesh::MarkGeometryDirty() {
  surfaceDirty = true;
  tetraDirty = true;
  boundingVolumeDirty = true;
  octreeDirty = true;
}

void QGLTetraMesh::UpdateSurfaceBuffers() {
  surfaceDirty = false;
  if (surf == NULL || surf->GetVertices().empty()) {
    surfaceSolidBuffer.destroy();
    surfaceWireframeBuffer.destroy();
    return;
  }
  const std::vector<Vec3f> &vertices = surf->GetVertices();
  const std::vector<Vec3f> &normals = surf->GetNormals();
  const std::vector<Triangle> &triangles = surf->GetTriangles();
  const std::vector<Edge> &edges = surf->GetEdges();
  const int numVertices = int(vertices.size());

  surfaceSolidBuffer.setPrimitive(GL_TRIANGLES);
  surfaceSolidBuffer.setAttribute(qtm::MeshBuffer::Position,
                                  toFloats(vertices), numVertices);
  if (normals.size() == vertices.size())
    surfaceSolidBuffer.setAttribute(qtm::MeshBuffer::Normal, toFloats(normals),
                                    numVertices);
  else
    surfaceSolidBuffer.removeAttribute(qtm::MeshBuffer::Normal);
  if (triangles.empty())
    surfaceSolidBuffer.destroy();
  else
    surfaceSolidBuffer.setIndices(&triangles[0].index[0],
                                  int(triangles.size() * 3));

  surfaceWireframeBuffer.setPrimitive(GL_LINES);
  surfaceWireframeBuffer.setAttribute(qtm::MeshBuffer::Position,
                                      toFloats(vertices), numVertices);
  if (edges.empty())
    surfaceWireframeBuffer.destroy();
  else
    surfaceWireframeBuffer.setIndices(&edges[0].index[0],
                                      int(edges.size() * 2));
}

void QGLTetraMesh::UpdateTetraBuffers() {
  tetraDirty = false;
  if (top == NULL || top->GetTetrahedra().empty()) {
    tetraSolidBuffer.destroy();
    tetraWireframeBuffer.destroy();
    return;
  }
  const std::vector<Vec3f> &vertices = top->GetVertices();
  const std::vector<Tetrahedron> &tetras = top->GetTetrahedra();
  const std::vector<Edge> &edges = top->GetEdges();

  tetraWireframeBuffer.setPrimitive(GL_LINES);
  tetraWireframeBuffer.setAttribute(qtm::MeshBuffer::Position,
                                    toFloats(vertices), int(vertices.size()));
  if (edges.empty())
    tetraWireframeBuffer.destroy();
  else
    tetraWireframeBuffer.setIndices(&edges[0].index[0], int(edges.size() * 2));

  // one instance per tetrahedron with its 4 corners as instance attributes
  const int numTetras = int(tetras.size());
  std::vector<Vec3f> corners(tetras.size());
  for (GLuint k = 0; k < 4; ++k) {
    for (int i = 0; i < numTetras; ++i)
      corners[i] = vertices[tetras[i].index[k]];
    tetraSolidBuffer.setAttribute(k, toFloats(corners), numTetras, 3, 1);
  }
  tetraSolidBuffer.setPrimitive(GL_TRIANGLES);
  tetraSolidBuffer.setVertexCount(12);
  tetraSolidBuffer.setInstanceCount(numTetras);
}

void QGLTetraMesh::UpdateBoundingVolumeBuffer() {
  boundingVolumeDirty = false;
  boundingVolumeBufferMode = bboxMode;
  if (bboxMode != 1 && bboxMode != 2) {
    boundingVolumeBuffer.destroy();
    return;
  }
  BoundingBox bb;
  BoundingCube c;
  if (surf) {
    bb = surf->GetBoundingBox();
    c = surf->GetBoundingCube();
  }
  if (top) {
    bb = top->GetBoundingBox();
    c = top->GetBoundingCube();
  }
  if (bboxMode == 2) {  // larger bounding cube
    bb.min = Vec3f(c.center.x - c.size / 2, c.center.y - c.size / 2,
                   c.center.z - c.size / 2);
    bb.max = Vec3f(c.center.x + c.size / 2, c.center.y + c.size / 2,
                   c.center.z + c.size / 2);
  }
  qtm::MeshData lines;
  qtm::primitives::appendBoxLines(lines, toQVector(bb.min), toQVector(bb.max));
  boundingVolumeBuffer.upload(lines, GL_LINES);
}

void QGLTetraMesh::UpdateOctreeBuffer() {
  octreeDirty = false;
  octreeBufferMode = octreeMode;
  if (oct == NULL || (octreeMode != 1 && octreeMode != 2)) {
    octreeBuffer.destroy();
    return;
  }
  // mode 2 shows the smallest leafs only
  const std::vector<OctreeNode *> &leafs =
      oct->getRootNode()->getLeafs(octreeMode == 2);
  qtm::MeshData lines;
  lines.positions.reserve(leafs.size() * 24);
  for (OctreeNode *on : leafs) {
    qtm::primitives::appendBoxLines(lines, toQVector(on->getMinBC()),
                                    toQVector(on->getMaxBC()));
  }
  octreeBuffer.upload(lines, GL_LINES);
}

void QGLTetraMesh::UpdateBuffers() {
  if (surfaceDirty) UpdateSurfaceBuffers();
  if (tetraDirty) UpdateTetraBuffers();
  if (boundingVolumeDirty || boundingVolumeBufferMode != bboxMode)
    UpdateBoundingVolumeBuffer();
  if (octreeDirty || octreeBufferMode != octreeMode) UpdateOctreeBuffer();
}

void QGLTetraMesh::DrawTetraSolid(const qtm::RenderContext &context_) {
  if (!tetraProgram || tetraSolidBuffer.isEmpty()) return;
  /// draw tetra when its centroid is between the front and back cutplanes
  const BoundingBox b = top->GetBoundingBox();
  const Vec3f dist = b.max - b.min;
  const QVector3D cutMin(b.max.x - dist.x * xUpperThreshold,
                         b.max.y - dist.y * yUpperThreshold,
                         b.max.z - dist.z * zUpperThreshold);
  const QVector3D cutMax(b.max.x - dist.x * xLowerThreshold,
                         b.max.y - dist.y * yLowerThreshold,
                         b.max.z - dist.z * zLowerThreshold);
  tetraProgram->bind();
  context_.setMatrixUniforms(*tetraProgram);
  tetraProgram->setUniformValue("u_cutMin", cutMin);
  tetraProgram->setUniformValue("u_cutMax", cutMax);
  tetraProgram->setUniformValue("u_shrink", 0.95f);
  tetraProgram->setUniformValue(
      "u_color",
      QVector3D(tetraColorSolid.x, tetraColorSolid.y, tetraColorSolid.z));
  tetraSolidBuffer.draw();
  tetraProgram->release();
}

void QGLTetraMesh::Draw(const qtm::RenderContext &context_) {
  if (!isReady) return;
  UpdateBuffers();
  QOpenGLExtraFunctions *gl = context_.gl();
  // push solid faces back a little so wireframes on top stay visible
  gl->glPolygonOffset(1.0f, 1.0f);

  /// draw tetra mesh
  if (top != NULL) {
    if (drawTetraWireframe)
      context_.drawLines(tetraWireframeBuffer, toQColor(tetraColorWireframe));
    if (drawTetraSolid) {
      gl->glEnable(GL_CULL_FACE);
      gl->glEnable(GL_POLYGON_OFFSET_FILL);
      DrawTetraSolid(context_);
    }
  }
  /// draw surface if possible
  switch (surfaceMode) {
    case 1:  // wireframe
      context_.drawLines(surfaceWireframeBuffer,
                         toQColor(surfaceColorWireframe));
      break;
    case 0:  // surface with normals
      gl->glEnable(GL_CULL_FACE);
      gl->glEnable(GL_POLYGON_OFFSET_FILL);
      context_.drawSolid(surfaceSolidBuffer, toQColor(surfaceColorSolid));
      break;
    default:  // 2 = hidden
      break;
  }
  gl->glDisable(GL_CULL_FACE);
  gl->glDisable(GL_POLYGON_OFFSET_FILL);

  /// draw bounding volume and octree
  context_.drawLines(boundingVolumeBuffer, QColor::fromRgbF(0.1f, 0.8f, 0.1f));
  context_.drawLines(octreeBuffer, QColor::fromRgbF(0.8f, 0.8f, 0.1f));
}

std::unique_ptr<TetraTools::TetrahedronTopology>
QGLTetraMesh::BuildTetraTopology(const std::vector<Vec3f> &verts_,
                                 const std::vector<Tetrahedron> &tetras_) {
  if (verts_.empty() || tetras_.empty() ||
      !TetraIndicesValid(verts_, tetras_)) {
    std::cerr << "Error! The tetrahedral mesh is empty or invalid."
              << std::endl;
    return nullptr;
  }
  std::unique_ptr<TetraTools::TetrahedronTopology> t(
      new TetraTools::TetrahedronTopology());
  // complete = false: the per-vertex maps, tetra edges etc. are not used by
  // the viewer; they are generated on first access if ever needed
  t->Init(verts_, tetras_, false);
  return t;
}

void QGLTetraMesh::SetTetraTopology(
    std::unique_ptr<TetraTools::TetrahedronTopology> top_) {
  isReady = false;
  delete top;
  top = top_.release();
  MarkGeometryDirty();
  isReady = (top != NULL || surf != NULL);
}

bool QGLTetraMesh::UpdateTetraMesh(std::vector<Vec3f> &verts_,
                                   std::vector<Tetrahedron> &tetras_) {
  // keeps showing the surface if there is no usable tetra mesh
  std::unique_ptr<TetraTools::TetrahedronTopology> t =
      BuildTetraTopology(verts_, tetras_);
  const bool valid = (t != nullptr);
  SetTetraTopology(std::move(t));
  return valid;
}

void QGLTetraMesh::SetOctree(Octree *oct_) {
  delete oct;
  oct = oct_;
  octreeDirty = true;
}

void QGLTetraMesh::ClearSurface() {
  isReady = false;
  delete oct;
  oct = NULL;
  delete surf;
  surf = NULL;
  MarkGeometryDirty();
  isReady = (top != NULL);
}

// The Load*() functions first load into temporary objects and replace the
// current mesh only on success, so a failed load keeps the current mesh.

bool QGLTetraMesh::LoadGMSH(const std::string &fileName_) {
  Timer t;
  t.start();
  TetraTools::GMSHMeshLoader gloader;
  if (!gloader.Load(fileName_)) {
    std::cerr << "Error loading GMSH file '" << fileName_ << "'." << std::endl;
    return false;
  }
  if (!TetraIndicesValid(gloader.GetVertices(), gloader.GetTetras())) {
    std::cerr << "Error! The GMSH file contains invalid vertex indices."
              << std::endl;
    return false;
  }
  std::unique_ptr<TetraTools::TetrahedronTopology> newTop(
      new TetraTools::TetrahedronTopology());
  newTop->Init(gloader.GetVertices(), gloader.GetTetras(), false);
  std::unique_ptr<TetraTools::TriangleTopology> newSurf(
      new TetraTools::TriangleTopology());
  newSurf->Init(newTop->GetVertices(), newTop->GetSurfaceTriangles(), true);
  newSurf->GenerateNormals();
  isReady = false;
  ClearAll();
  top = newTop.release();
  surf = newSurf.release();
  isReady = true;
  t.stop();
  std::cout << "Finished loading GMSH TetraMesh in "
            << t.getElapsedTimeInMilliSec() << " ms." << std::endl;
  return true;
}

bool QGLTetraMesh::LoadSurface(const std::string &fileName_, bool repair_) {
  Timer t;
  t.start();
  TetraTools::TriMeshLoader gloader;
  if (!gloader.loadFile(fileName_, repair_)) {
    std::cerr << "Error loading file '" << fileName_ << "'." << std::endl;
    return false;
  }
  std::unique_ptr<TetraTools::TriangleTopology> newSurf(
      new TetraTools::TriangleTopology());
  newSurf->Init(gloader.GetVertices(), gloader.GetTriangles(), true);
  newSurf->GenerateNormals();
  const MeshRepairReport &repair = gloader.GetRepairReport();
  std::string summary = repair.Summary();
  if (repair.numBoundaryEdges || repair.numNonManifoldEdges) {
    if (!summary.empty()) summary += "; ";
    summary += "still " + std::to_string(repair.numBoundaryEdges) +
               " open and " + std::to_string(repair.numNonManifoldEdges) +
               " non-manifold edges";
  }
  isReady = false;
  ClearAll();
  surfaceRepairSummary = summary;
  surfaceRepaired = repair.Changed();
  surf = newSurf.release();
  isReady = true;
  t.stop();
  std::cout << "Finished loading SurfaceMesh in "
            << t.getElapsedTimeInMilliSec() << " ms." << std::endl;
  return true;
}

bool QGLTetraMesh::LoadTetgen(const std::string &fileName_) {
  Timer t;
  t.start();
  TetgenWrapper tloader;
  if (!tloader.loadAsTetgen(std::string(), fileName_)) {
    std::cerr << "Error loading Tetgen file '" << fileName_ << "'." << std::endl;
    return false;
  }
  if (!TetraIndicesValid(tloader.GetTetraVertices(), tloader.GetTetras())) {
    std::cerr << "Error! The Tetgen files contain invalid vertex indices."
              << std::endl;
    return false;
  }
  std::unique_ptr<TetraTools::TetrahedronTopology> newTop(
      new TetraTools::TetrahedronTopology());
  newTop->Init(tloader.GetTetraVertices(), tloader.GetTetras(), false);
  std::unique_ptr<TetraTools::TriangleTopology> newSurf(
      new TetraTools::TriangleTopology());
  newSurf->Init(newTop->GetVertices(), newTop->GetSurfaceTriangles(), true);
  newSurf->GenerateNormals();
  isReady = false;
  ClearAll();
  top = newTop.release();
  surf = newSurf.release();
  isReady = true;
  t.stop();
  std::cout << "Finished loading Tetgen TetraMesh in "
            << t.getElapsedTimeInMilliSec() << " ms." << std::endl;
  return true;
}

bool QGLTetraMesh::SaveGMSH(const std::string &fileName_) {
  if (top == NULL) return false;
  Timer t;
  t.start();
  TetraTools::GMSHMeshWriter gwriter;
  bool success =
      (gwriter.SaveToFile(fileName_, top->GetVertices(), top->GetTetrahedra()));
  t.stop();
  std::cout << "Finished saving GMSH in " << t.getElapsedTimeInMilliSec()
            << " ms." << std::endl;
  return success;
}

bool QGLTetraMesh::SaveTetgen(const std::string &fileName_) {
  if (top == NULL) return false;
  Timer t;
  t.start();
  TetgenWrapper twriter;
  // fileName_ is the extension-less prefix; saveAsTetgen() will append the
  // required ".node"/".ele" extensions using Tetgen's own file writers.
  bool success = twriter.saveAsTetgen(std::string(), fileName_,
                                      top->GetTetrahedra(), top->GetVertices());
  t.stop();
  std::cout << "Finished saving Tetgen in " << t.getElapsedTimeInMilliSec()
            << " ms." << std::endl;
  return success;
}

bool QGLTetraMesh::SaveSurface(const std::string &fileName_,
                               const std::string extension_) {
  if (surf == NULL) return false;
  Timer t;
  t.start();
  TetraTools::TriMeshWriter twriter;
  bool success = twriter.writeFile(fileName_, extension_, surf->GetVertices(),
                                   surf->GetTriangles());
  t.stop();
  std::cout << "Finished saving Surface in " << t.getElapsedTimeInMilliSec()
            << " ms." << std::endl;
  return success;
}

void QGLTetraMesh::ScaleMesh(const float factor_) {
  if (surf == NULL) {
    std::cerr << "Error on scale! Surface mesh is empty..." << std::endl;
    return;
  }
  isReady = false;
  for (Vec3f &v : surf->GetVertices()) {
    v *= factor_;
  }
  surf->GenerateBoundingBox();
  // the octree was built from the unscaled surface
  delete oct;
  oct = NULL;
  if (top != NULL) {
    std::vector<Vec3f> tv = top->GetVertices();
    std::vector<Tetrahedron> tt = top->GetTetrahedra();
    for (Vec3f &v : tv) {
      v *= factor_;
    }
    UpdateTetraMesh(tv, tt);
  }
  MarkGeometryDirty();
  isReady = true;
}

void QGLTetraMesh::scalex10() { ScaleMesh(10.0f); }

void QGLTetraMesh::scalex01() { ScaleMesh(0.1f); }

void QGLTetraMesh::generateOctree(const unsigned int depth_) {
  if (oct != NULL) {
    delete oct;
    oct = NULL;
  }
  if (surf == NULL) {
    std::cerr << "Error on Octree generation: Surface is empty..." << std::endl;
    return;
  }
  Timer t;
  t.start();
  oct = new Octree(depth_, &(surf->GetVertices()), &(surf->GetTriangles()));
  octreeDirty = true;
  t.stop();
  std::cout << "Octree Level " << oct->getDepth() << " generated in "
            << t.getElapsedTimeInMilliSec() << " ms." << std::endl;
}

void QGLTetraMesh::SetXRange(int lower, int upper) {
  xLowerThreshold = (float)lower / 100.0f;
  xUpperThreshold = (float)upper / 100.0f;
}

void QGLTetraMesh::SetYRange(int lower, int upper) {
  yLowerThreshold = (float)lower / 100.0f;
  yUpperThreshold = (float)upper / 100.0f;
}

void QGLTetraMesh::SetZRange(int lower, int upper) {
  zLowerThreshold = (float)lower / 100.0f;
  zUpperThreshold = (float)upper / 100.0f;
}

void QGLTetraMesh::translateSurfaceMesh(const Vec3f offset_) {
  if (surf == NULL) return;
  for (auto &it : surf->GetVertices()) {
    it += offset_;
  }
  // keep the bounding box in sync with the moved vertices
  surf->GenerateBoundingBox();
  MarkGeometryDirty();
}
