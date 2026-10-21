/*
 * QGLTetraMesh.h
 *
 *  Created on: Sep 11, 2011
 *      Author: Dennis Luebke
 *
 *	Drawing class for tetrahedral meshes including visualization
 *	for edges, wireframe and solid tetrahedra, cutplane tetrahedral mesh,
 *	surface mesh and bounding volume.
 *	Might once be extended for visualizing an octree and eventually a BCC
 *(body centered cubic)...
 *
 *	This class will also handle events forloading and saving files and
 *manage the underlying topology.
 *
 */

#ifndef QGLTETRAMESH_H_
#define QGLTETRAMESH_H_

#ifdef BUILD_QGRID
#ifdef Q_CC_MSVC
#define QGRID_EXPORT Q_DECL_EXPORT
#else
#define QGRID_EXPORT Q_DECL_IMPORT
#endif
#else
#define QGRID_EXPORT
#endif

#include <QtGui/QColor>
#include <QtmGLViewer/MeshBuffer.hpp>
#include <QtmGLViewer/RenderContext.hpp>
#include <memory>
#include <string>
#include <vector>

#include "TetraMeshTools/Octree.h"
#include "TetraMeshTools/TetrahedronTopology.h"

class QGLTetraMesh {
 private:
  TetraTools::TetrahedronTopology *top;  /// tetra mesh topology
  TetraTools::TriangleTopology *surf;    /// triangle mesh for surface
  bool drawTetraMesh;
  bool drawTriangleMesh;
  bool drawTetraSurface;
  bool drawSolid;
  bool showBoundingBox;
  bool isReady;
  int bboxMode;  // 0 = hide, 1 = bounding box
  bool drawTetraSolid;
  bool drawTetraWireframe;
  int surfaceMode;  // 0 = solid, 1 = wireframe, 2 = hidden
  int octreeMode;   // 0 = hidden, 1 = visible

  float xLowerThreshold;
  float xUpperThreshold;
  float yLowerThreshold;
  float yUpperThreshold;
  float zLowerThreshold;
  float zUpperThreshold;

  /// uploads all geometry marked as dirty to the GPU
  void ClearAll();  // deletes octree, tetra mesh and surface
  void ScaleMesh(const float factor_);
  void UpdateBuffers();
  void UpdateSurfaceBuffers();
  void UpdateTetraBuffers();
  void UpdateBoundingVolumeBuffer();
  void UpdateOctreeBuffer();
  void DrawTetraSolid(const qtm::RenderContext &context_);

  /// GPU buffers, (re-)uploaded lazily in Draw() when marked dirty
  qtm::MeshBuffer surfaceSolidBuffer;
  qtm::MeshBuffer surfaceWireframeBuffer;
  qtm::MeshBuffer tetraSolidBuffer;  // one instance per tetrahedron
  qtm::MeshBuffer tetraWireframeBuffer;
  qtm::MeshBuffer boundingVolumeBuffer;
  qtm::MeshBuffer octreeBuffer;
  std::unique_ptr<QOpenGLShaderProgram> tetraProgram;
  bool surfaceDirty;
  bool tetraDirty;
  bool boundingVolumeDirty;
  bool octreeDirty;
  int boundingVolumeBufferMode;  // bboxMode the buffer was built for
  int octreeBufferMode;          // octreeMode the buffer was built for

  Vec3f surfaceColorSolid;
  Vec3f tetraColorSolid;  // tetrahedra with a face on the mesh boundary
  Vec3f tetraColorOuter;  // interior gradient: next to the boundary ...
  Vec3f tetraColorInner;  // ... to the deepest tetrahedra
  bool tetraDepthColoring;
  Vec3f surfaceColorWireframe;
  Vec3f tetraColorWireframe;

  Octree *oct;

 public:
  QGLTetraMesh();
  ~QGLTetraMesh();

  TetraTools::TriangleTopology *GetSurface() { return surf; }

  TetraTools::TetrahedronTopology *GetTetraMesh() { return top; }

  /// Draws all visible meshes. Requires the viewer's OpenGL context.
  void Draw(const qtm::RenderContext &context_);
  /// Creates/releases GPU resources (OpenGL context must be current)
  void InitGL();
  void CleanupGL();
  /// Forces a re-upload of all geometry on the next Draw(). Call this after
  /// modifying the surface or tetra mesh directly.
  void MarkGeometryDirty();
  /// Loading functions return false and keep the current mesh on failure.
  /// (GMSH files are loaded by a TetraJob, see SetLoadedTetraMesh().)
  bool LoadTetgen(const std::string &fileName_);
  /// With repair_ set, defects of the surface are fixed where possible
  /// (see RepairSurfaceMesh()).
  bool LoadSurface(const std::string &fileName_, bool repair_ = true);
  /// true if the repair of the last LoadSurface() changed the surface
  bool WasSurfaceRepaired() const { return surfaceRepaired; }
  /// what the repair of the last LoadSurface() changed and what is still
  /// broken (empty if the surface was fine or the repair was off)
  const std::string &GetSurfaceRepairSummary() const {
    return surfaceRepairSummary;
  }
  /// mshVersion_ is the MSH format version, e.g. 4.1 or 2.2
  bool SaveGMSH(const std::string &fileName_, double mshVersion_ = 4.1);
  bool SaveSurface(const std::string &fileName_, const std::string extension_);
  bool SaveTetgen(const std::string &fileName_);

  void scalex10();

 private:
  std::string surfaceRepairSummary;
  bool surfaceRepaired = false;

 public:

  void scalex01();

  /// Returns false (and drops the tetra mesh) if the data is empty or invalid.
  bool UpdateTetraMesh(std::vector<Vec3f> &verts_,
                       std::vector<Tetrahedron> &tetras_);

  /// Builds the topology for a tetra mesh. Returns null if the data is empty
  /// or invalid. Does not use any QGLTetraMesh state, so it may run in a
  /// background thread.
  static std::unique_ptr<TetraTools::TetrahedronTopology> BuildTetraTopology(
      const std::vector<Vec3f> &verts_,
      const std::vector<Tetrahedron> &tetras_);

  /// Replaces the tetra mesh (null removes it) and takes ownership.
  void SetTetraTopology(std::unique_ptr<TetraTools::TetrahedronTopology> top_);

  /// Replaces everything by a loaded tetra mesh and its surface (both built
  /// in the background, e.g. by a TetraJob) and takes ownership.
  void SetLoadedTetraMesh(
      std::unique_ptr<TetraTools::TetrahedronTopology> top_,
      std::unique_ptr<TetraTools::TriangleTopology> surf_);

  /// Replaces the octree (null removes it) and takes ownership. The octree
  /// must have been built from the current surface mesh.
  void SetOctree(Octree *oct_);

  void ClearSurface();

  int GetBoundingMode() { return bboxMode; }

  int GetSurfaceMode() { return surfaceMode; }

  bool GetDrawTetraMesh() { return drawTetraMesh; }

  bool GetDrawTriangleMesh() { return drawTriangleMesh; }

  bool GetDrawTetraSurface() { return drawTetraSurface; }

  bool GetDrawSolid() { return drawSolid; }

  bool GetShowBBox() { return showBoundingBox; }

  void ToggleBBox(int i) { bboxMode = i; }

  void ToggleTetraMesh(bool drawWireframe_, bool drawSolid_) {
    drawTetraWireframe = drawWireframe_;
    drawTetraSolid = drawSolid_;
  }

  void ToggleOctreeVis(int i) { octreeMode = i; }

  void ToggleDrawSolid() { drawSolid = !drawSolid; }

  void ToggleTriangleMesh(int i) { surfaceMode = i; }

  void ToggleTetraSurface() { drawTetraSurface = !drawTetraSurface; }

  void SetXRange(int lower, int upper);
  void SetYRange(int lower, int upper);
  void SetZRange(int lower, int upper);

  float GetSceneRadius() { return surf->GetRadius(); }

  bool IsReady() { return isReady; }

  BoundingBox GetBoundingBox() {
    if (surf) return surf->GetBoundingBox();
    if (top) return top->GetBoundingBox();
    BoundingBox bb;
    bb.min = Vec3f(0, 0, 0);
    bb.max = Vec3f(0, 0, 0);
    return bb;
  }

  BoundingCube GetBoundingCube() {
    if (surf) return surf->GetLargeBoundingCube();
    if (top) return top->GetLargeBoundingCube();
    BoundingCube bc;
    bc.size = 1.0f;
    bc.center = Vec3f(0, 0, 0);
    return bc;
  }

  TetraTools::TriangleTopology *GetTriangleTopology() { return surf; }

  TetraTools::TetrahedronTopology *GetTetraTopology() { return top; }

  void SetTetraSolidColor(float r, float g, float b) {
    tetraColorSolid.x = r;
    tetraColorSolid.y = g;
    tetraColorSolid.z = b;
    tetraColorSolid /= 255.0f;
  }

  void SetTetraOuterColor(float r, float g, float b) {
    tetraColorOuter = Vec3f(r, g, b) / 255.0f;
  }

  void SetTetraInnerColor(float r, float g, float b) {
    tetraColorInner = Vec3f(r, g, b) / 255.0f;
  }

  /// Colors interior tetrahedra by their distance from the boundary. If off,
  /// all tetrahedra use the tetra color.
  void SetTetraDepthColoring(bool enabled_) { tetraDepthColoring = enabled_; }

  bool GetTetraDepthColoring() const { return tetraDepthColoring; }

  void SetSurfaceSolidColor(float r, float g, float b) {
    surfaceColorSolid.x = r;
    surfaceColorSolid.y = g;
    surfaceColorSolid.z = b;
    surfaceColorSolid /= 255.0f;
  }

  void SetTetraWireframeColor(float r, float g, float b) {
    tetraColorWireframe.x = r;
    tetraColorWireframe.y = g;
    tetraColorWireframe.z = b;
    tetraColorWireframe /= 255.0f;
  }

  void SetSurfaceWireframeColor(float r, float g, float b) {
    surfaceColorWireframe.x = r;
    surfaceColorWireframe.y = g;
    surfaceColorWireframe.z = b;
    surfaceColorWireframe /= 255.0f;
  }

  // Colors are rounded (not truncated) so that a color survives repeated
  // get/set round trips, e.g. when saving and restoring settings.
  static QColor ToQColor(const Vec3f &c) {
    return QColor(qRound(c.x * 255.0f), qRound(c.y * 255.0f),
                  qRound(c.z * 255.0f));
  }

  const QColor GetSurfaceColor() { return ToQColor(surfaceColorSolid); }

  const QColor GetSurfaceWireframeColor() {
    return ToQColor(surfaceColorWireframe);
  }

  const QColor GetTetraColor() { return ToQColor(tetraColorSolid); }

  const QColor GetTetraOuterColor() { return ToQColor(tetraColorOuter); }

  const QColor GetTetraInnerColor() { return ToQColor(tetraColorInner); }

  const QColor GetTetraWireframeColor() {
    return ToQColor(tetraColorWireframe);
  }

  void generateOctree(const unsigned int depth_);

  void translateSurfaceMesh(const Vec3f offset_);
};

#endif /* QGLTETRAMESH_H_ */
