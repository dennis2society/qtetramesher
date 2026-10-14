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
      xLowerThreshold(0.01f),
      xUpperThreshold(1.0f),
      yLowerThreshold(0.01f),
      yUpperThreshold(1.0f),
      zLowerThreshold(0.01f),
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
  if (top != NULL) {
    delete top;
  }
  if (surf != NULL) {
    delete surf;
  }
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

void QGLTetraMesh::UpdateTetraMesh(std::vector<Vec3f> &verts_,
                                   std::vector<Tetrahedron> &tetras_) {
  isReady = false;
  if (top != NULL) {
    delete top;
    top = NULL;
  }
  if (verts_.size() == 0 || tetras_.size() == 0) {
    isReady = false;
    return;
  }
  top = new TetraTools::TetrahedronTopology();
  top->Init(verts_, tetras_, true);
  MarkGeometryDirty();
  isReady = true;
}

void QGLTetraMesh::ClearSurface() {
  if (surf != NULL) {
    delete surf;
    surf = NULL;
  }
  if (oct != NULL) {
    delete oct;
    oct = NULL;
  }
  MarkGeometryDirty();
}

void QGLTetraMesh::LoadGMSH(const std::string &fileName_) {
  isReady = false;
  if (top != NULL) {
    delete top;
    top = NULL;
  }
  if (surf != NULL) {
    delete surf;
    surf = NULL;
  }
  if (oct != NULL) {
    delete oct;
    oct = NULL;
  }
  Timer t;
  t.start();
  TetraTools::GMSHMeshLoader *gloader = new TetraTools::GMSHMeshLoader();
  if (!gloader->Load(fileName_)) {
    std::cerr << "Error loading GMSH file '" << fileName_ << "'." << std::endl;
    delete gloader;
    return;
  }
  top = new TetraTools::TetrahedronTopology();
  top->Init(gloader->GetVertices(), gloader->GetTetras(), true);
  surf = new TetraTools::TriangleTopology();
  surf->Init(top->GetVertices(), top->GetSurfaceTriangles(), true);
  surf->GenerateNormals();
  MarkGeometryDirty();
  isReady = true;
  t.stop();
  std::cout << "Finished loading GMSH TetraMesh in "
            << t.getElapsedTimeInMilliSec() << " ms." << std::endl;
  delete gloader;
}

void QGLTetraMesh::LoadSurface(const std::string &fileName_) {
  isReady = false;
  if (surf != NULL) {
    delete surf;
    surf = NULL;
  }
  if (top != NULL) {
    delete top;
    top = NULL;
  }
  if (oct != NULL) {
    delete oct;
    oct = NULL;
  }
  Timer t;
  t.start();
  TetraTools::TriMeshLoader gloader;
  if (!gloader.loadFile(fileName_)) {
    std::cerr << "Error loading file '" << fileName_ << "'." << std::endl;
    return;
  }
  surf = new TetraTools::TriangleTopology();
  surf->Init(gloader.GetVertices(), gloader.GetTriangles(), true);
  surf->GenerateNormals();
  MarkGeometryDirty();
  isReady = true;
  t.stop();
  std::cout << "Finished loading SurfaceMesh in "
            << t.getElapsedTimeInMilliSec() << " ms." << std::endl;
}

void QGLTetraMesh::LoadTetgen(const std::string &fileName_) {
  isReady = false;
  if (top != NULL) {
    delete top;
    top = NULL;
  }
  if (surf != NULL) {
    delete surf;
    surf = NULL;
  }
  if (oct != NULL) {
    delete oct;
    oct = NULL;
  }
  Timer t;
  t.start();
  TetgenWrapper tloader;
  if (!tloader.loadAsTetgen(std::string(), fileName_)) {
    std::cerr << "Error loading Tetgen file '" << fileName_ << "'." << std::endl;
    return;
  }
  top = new TetraTools::TetrahedronTopology();
  top->Init(tloader.GetTetraVertices(), tloader.GetTetras(), true);
  surf = new TetraTools::TriangleTopology();
  surf->Init(top->GetVertices(), top->GetSurfaceTriangles(), true);
  surf->GenerateNormals();
  MarkGeometryDirty();
  isReady = true;
  t.stop();
  std::cout << "Finished loading Tetgen TetraMesh in "
            << t.getElapsedTimeInMilliSec() << " ms." << std::endl;
}

bool QGLTetraMesh::SaveGMSH(const std::string &fileName_) {
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

void QGLTetraMesh::scalex10() {
  isReady = false;
  if (surf == NULL) {
    std::cerr << "Error on scale! Surface mesh is empty..." << std::endl;
    return;
  } else {
    for (unsigned int i = 0; i < surf->GetVertices().size(); ++i) {
      Vec3f &v = surf->GetVertices()[i];
      v *= 10.0f;
    }
    surf->GenerateBoundingBox();
  }
  if (top == NULL) {
    std::cerr << "Error on scale! Tetra mesh is empty..." << std::endl;
  } else {
    std::vector<Vec3f> tv;
    std::vector<Tetrahedron> tt;
    for (unsigned int i = 0; i < top->GetVertices().size(); ++i) {
      Vec3f v = top->GetVertices()[i];
      v *= 10.0f;
      tv.push_back(v);
    }
    for (unsigned int i = 0; i < top->GetTetrahedra().size(); ++i) {
      tt.push_back(top->GetTetrahedra()[i]);
    }
    UpdateTetraMesh(tv, tt);
  }
  MarkGeometryDirty();
  isReady = true;
}

void QGLTetraMesh::scalex01() {
  isReady = false;
  if (surf == NULL) {
    std::cerr << "Error on scale! Surface mesh is empty..." << std::endl;
    return;
  } else {
    for (unsigned int i = 0; i < surf->GetVertices().size(); ++i) {
      Vec3f &v = surf->GetVertices()[i];
      v *= 0.1f;
    }
    surf->GenerateBoundingBox();
  }
  if (top == NULL) {
    std::cerr << "Error on scale! Tetra mesh is empty..." << std::endl;
  } else {
    std::vector<Vec3f> tv;
    std::vector<Tetrahedron> tt;
    for (unsigned int i = 0; i < top->GetVertices().size(); ++i) {
      Vec3f v = top->GetVertices()[i];
      v *= 0.1f;
      tv.push_back(v);
    }
    for (unsigned int i = 0; i < top->GetTetrahedra().size(); ++i) {
      tt.push_back(top->GetTetrahedra()[i]);
    }
    UpdateTetraMesh(tv, tt);
  }
  MarkGeometryDirty();
  isReady = true;
}

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
  for (auto &it : surf->GetVertices()) {
    it += offset_;
  }
  MarkGeometryDirty();
}
