/*
 * CDTWrapper.h
 *
 * Wrapper for the constrained Delaunay tetrahedrization (CDT) by Diazzi,
 * Panozzo, Vaxman and Attene. https://github.com/MarcoAttene/CDT
 */

#pragma once

#include <TetraMeshTools/GeometryTypes.h>
#include <TetraMeshTools/TetraToolsExports.h>
#include <cstddef>
#include <vector>

class DLL_EXPORT CDTWrapper {
public:
  /**
   * Tetrahedralizes the surface with a constrained Delaunay tetrahedrization.
   * The input triangles are kept exactly; Steiner points are only added where
   * the surface does not admit a CDT. Throws std::runtime_error on failure.
   *
   * @param innerOnly        keep only the tetrahedra inside the surface
   *                         (ignored if the surface is not closed)
   * @param boundingBox      add the 8 corners of an enlarged bounding box
   * @param interiorSpacing  > 0 adds a regular grid of interior points with
   *                         this spacing (CDT does no refinement by itself)
   * @param verbose          print the progress of the algorithm
   */
  void GenerateFromSurface(const std::vector<Triangle> &tris,
                           const std::vector<Vec3f> &verts, bool innerOnly,
                           bool boundingBox, double interiorSpacing,
                           bool verbose);

  std::vector<Vec3f> &GetTetraVertices() { return tetraPoints; }
  std::vector<Tetrahedron> &GetTetras() { return tetraIndices; }

private:
  std::vector<Vec3f> tetraPoints;
  std::vector<Tetrahedron> tetraIndices;
};
