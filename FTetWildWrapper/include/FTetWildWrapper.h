/*
 * FTetWildWrapper.h
 *
 * This is a wrapper interface for the fTetWild library.
 * https://github.com/wildmeshing/fTetWild
 *
 * The module is optional, see QTM_WITH_FTETWILD in the main CMakeLists.txt.
 */

#pragma once

#include <TetraMeshTools/GeometryTypes.h>
#include <TetraMeshTools/TetraToolsExports.h>
#include <vector>

class DLL_EXPORT FTetWildWrapper {
 public:
  FTetWildWrapper();
  ~FTetWildWrapper();

  /**
   * Generates a tetrahedral mesh from a triangle surface. fTetWild also
   * handles surfaces with holes or self-intersections.
   *
   * @param edgeLengthRel ideal edge length relative to the bounding box
   *                      diagonal
   * @param epsilonRel    allowed distance from the input surface relative to
   *                      the bounding box diagonal
   * @param stopEnergy    the optimization stops below this max. energy
   * @param coarsen       coarsen the output as much as possible
   */
  void GenerateFromSurface(const std::vector<Triangle> &tris,
                           const std::vector<Vec3f> &verts,
                           double edgeLengthRel, double epsilonRel,
                           double stopEnergy, bool coarsen);

  std::vector<Vec3f> &GetTetraVertices() { return tetraPoints; }
  std::vector<Tetrahedron> &GetTetras() { return tetraIndices; }

 private:
  std::vector<Vec3f> tetraPoints;
  std::vector<Tetrahedron> tetraIndices;
};
