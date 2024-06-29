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
  int bboxMode;     // 0 = hide, 1 = bounding box
  int tetraMode;    // 0 = solid, 1 = wireframe, 2 = hidden
  int surfaceMode;  // 0 = solid, 1 = wireframe, 2 = hidden
  int octreeMode;   // 0 = hidden, 1 = visible

  float xLowerThreshold;
  float xUpperThreshold;
  float yLowerThreshold;
  float yUpperThreshold;
  float zLowerThreshold;
  float zUpperThreshold;

  void DrawBoundingVolume(const BoundingBox &bb, Vec3f color_);

  void DrawTetraTriangle(const unsigned int triangleIndex,
                         const Vec3f &tetraCentroid, const Vec3f &color);

  void DrawTetrahedron(const unsigned int tetraIndex);

  void DrawOctree(const bool leafsOnyly_);

  Vec3f surfaceColorSolid;
  Vec3f tetraColorSolid;
  Vec3f surfaceColorWireframe;
  Vec3f tetraColorWireframe;

  Octree *oct;

 public:
  QGLTetraMesh();
  ~QGLTetraMesh();

  TetraTools::TriangleTopology *GetSurface() { return surf; }

  TetraTools::TetrahedronTopology *GetTetraMesh() { return top; }

  void Draw();
  void LoadGMSH(const std::string &fileName_);
  void LoadTetgen(const std::string &fileName_);
  void LoadSurface(const std::string &fileName_);
  bool SaveGMSH(const std::string &fileName_);
  bool SaveSurface(const std::string &fileName_, const std::string extension_);
  bool SaveTetgen(const std::string &fileName_);

  void scalex10();

  void scalex01();

  void UpdateTetraMesh(std::vector<Vec3f> &verts_,
                       std::vector<Tetrahedron> &tetras_);

  void ClearSurface();

  int GetBoundingMode() { return bboxMode; }

  int GetTetraMode() { return tetraMode; }

  int GetSurfaceMode() { return surfaceMode; }

  bool GetDrawTetraMesh() { return drawTetraMesh; }

  bool GetDrawTriangleMesh() { return drawTriangleMesh; }

  bool GetDrawTetraSurface() { return drawTetraSurface; }

  bool GetDrawSolid() { return drawSolid; }

  bool GetShowBBox() { return showBoundingBox; }

  void ToggleBBox(int i) { bboxMode = i; }

  void ToggleTetraMesh(int i) { tetraMode = i; }

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

  const QColor GetSurfaceColor() {
    QColor c((int)(surfaceColorSolid.x * 255.0f),
             (int)(surfaceColorSolid.y * 255.0f),
             (int)(surfaceColorSolid.z * 255.0f));
    return c;
  }

  const QColor GetSurfaceWireframeColor() {
    return QColor(surfaceColorWireframe.x * 255.0f,
                  surfaceColorWireframe.y * 255.0f,
                  surfaceColorWireframe.z * 255.0f);
  }

  const QColor GetTetraColor() {
    return QColor(tetraColorSolid.x * 255.0f, tetraColorSolid.y * 255.0f,
                  tetraColorSolid.z * 255.0f);
  }

  const QColor GetTetraWireframeColor() {
    return QColor(tetraColorWireframe.x * 255.0f,
                  tetraColorWireframe.y * 255.0f,
                  tetraColorWireframe.z * 255.0f);
  }

  void generateOctree(const unsigned int depth_);

  void translateSurfaceMesh(const Vec3f offset_);
};

#endif /* QGLTETRAMESH_H_ */
