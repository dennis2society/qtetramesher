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

  /// Saves a tetrahedral mesh as a GMSH .msh file using GMSH's own writer.
  /// Only linear tetrahedra are written (one volume entity, physical group
  /// 1). mshVersion selects the file format, e.g. 4.1 (GMSH default) or 2.2
  /// for older readers. The file name must end with ".msh".
  bool saveAsGMSH(const std::string &fileName,
                  const std::vector<Tetrahedron> &tetras,
                  const std::vector<Vec3f> &verts, double mshVersion = 4.1);

  /// Loads the tetrahedra of a GMSH .msh file (any version GMSH can read,
  /// ASCII or binary) using GMSH's own reader. Other element types are
  /// ignored, as are nodes not used by a tetrahedron. The result is stored
  /// internally and accessible via GetTetraVertices() / GetTetras().
  bool loadAsGMSH(const std::string &fileName);

  /// Why the last loadAsGMSH() failed (empty after a successful load)
  const std::string &GetLastError() const { return lastError; }

private:
  std::vector<Vec3f> tetraPoints;
  std::vector<Tetrahedron> tetraIndices;
  double usedMeshSizeMin = 0.0;
  double usedMeshSizeMax = 0.0;
  std::string lastError;
};
