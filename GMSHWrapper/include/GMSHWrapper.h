/*
 * GMSHWrapper.h
 *
 * This is a wrapper interface for the GMSH library.
 * https://gmsh.info/
 *
 *  Created on: 2024
 *      Author: Dennis Luebke
 */

#pragma once

#include <TetraMeshTools/GeometryTypes.h>
#include <TetraMeshTools/TetraToolsExports.h>
#include <string>
#include <vector>

class DLL_EXPORT GMSHWrapper {
public:
  GMSHWrapper();
  ~GMSHWrapper();

  /**
   * Main function: Generate a tetrahedral mesh from a closed triangle surface
   * defined by a list of triangles and a list of vertices using GMSH.
   *
   * @param tris         Input surface triangles
   * @param verts        Input surface vertices
   * @param meshSizeMin  Minimum mesh element size (<=0 triggers auto-compute)
   * @param meshSizeMax  Maximum mesh element size (<=0 triggers auto-compute)
   * @param algorithm3D  3D meshing algorithm: 1=Delaunay, 10=HXT
   *                     (4=Frontal needs a GMSH build with Netgen)
   */
  void GenerateFromSurface(const std::vector<Triangle> &tris,
                           const std::vector<Vec3f> &verts,
                           double meshSizeMin,
                           double meshSizeMax,
                           int algorithm3D);

  /** Returns the generated tetrahedra vertices */
  std::vector<Vec3f> &GetTetraVertices();

  /** Returns the generated tetrahedra indices */
  std::vector<Tetrahedron> &GetTetras();

  /** Mesh sizes used by the last GenerateFromSurface() call (after the
   *  automatic computation of values <= 0) */
  double GetMeshSizeMin() const { return usedMeshSizeMin; }
  double GetMeshSizeMax() const { return usedMeshSizeMax; }

private:
  std::vector<Vec3f> tetraPoints;
  std::vector<Tetrahedron> tetraIndices;
  double usedMeshSizeMin = 0.0;
  double usedMeshSizeMax = 0.0;
};
