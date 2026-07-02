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
  TetgenWrapper();

  ~TetgenWrapper();

  /**
   *  Main function: Generate a tetrahedral mesh from a triangle surface
   *  defined by a list of triangles and list of vertices.
   *  Additionally we will set all necessary parameters for Tetgen
   *  Also the output data containers will be filled here after processing.
   */
  void GenerateFromSurface(const std::vector<Triangle> &tris,
                           const std::vector<Vec3f> &verts, float qualityBounds,
                           float cellSize, bool usePLC);

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

  /// This function will load a Tetgen mesh (consisting of a .node and a .ele
  /// file). The resulting vertices/tetrahedra are stored internally and
  /// accessible via GetTetraVertices() / GetTetras().
  bool loadAsTetgen(const std::string path, const std::string baseName);

private:
  std::vector<Vec3f> tetraPoints;
  std::vector<Tetrahedron> tetraIndices;

  bool m_usePLC;
  float m_qualityBound;
  float m_cellSize;   ///< 0 or negative triggers auto-compute from bounding box
  bool m_noSubDivide;

  std::string generateTetgenParamString();
};
