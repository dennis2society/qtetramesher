/*
 * TetgenWrapper.h
 *
 * This is a wrapper interface for the Tetgen library.
 * https://wias-berlin.de/software/index.jsp?id=TetGen&lang=1
 *
 *  Created on: Oct 07, 2023
 *      Author: Dennis Luebke
 */

#pragma once

#include <TetraMeshTools/GeometryTypes.h>
#include <TetraMeshTools/TetraToolsExports.h>
#include <string>
#include <vector>

class DLL_EXPORT TetgenWrapper {
public:
  TetgenWrapper() {}

  ~TetgenWrapper();

  /**
   *  Main function: Generate a tetrahedral mesh from a triangle surface
   *  defined by a list of triangles and list of vertices.
   *  Additionally we will set all necessary parameters for Tetgen
   *  Also the output data containers will be filled here after processing.
   */
  void GenerateFromSurface(const std::vector<Triangle> &tris,
                           const std::vector<Vec3f> &verts, float qualityBounds,
                           float volumeConstraint, bool usePLC);

  /**
   * Returns the generated tetrahedra vertices
   */
  std::vector<Vec3f> &GetTetraVertices();

  /**
   *  Returns the generated tetrahedra tetra indices
   */
  std::vector<Tetrahedron> &GetTetras();

  /// This function will save a Tetgen mesh (consisting of a .node and a .ele
  /// file)
  bool saveAsTetgen(const std::string path, const std::string baseName,
                    const std::vector<Tetrahedron> &tetras,
                    const std::vector<Vec3f> &verts);

private:
  std::vector<Vec3f> tetraPoints;
  std::vector<Tetrahedron> tetraIndices;
};
