/*
 * GenericIsosurfaceStuffing.cpp
 *
 * Wrapper that delegates to the TetStuffer pipeline.
 *
 *  Created 2026
 */

#include "GenericIsosurfaceStuffing.h"
#include "TetStuffer.h"

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

void GenericIsosurfaceStuffing::GenerateFromSurface(
    const std::vector<Triangle> &tris,
    const std::vector<Vec3f> &verts,
    float gridSpacing,
    float alphaShort,
    float alphaLong,
    bool graded) {

  tetraPoints.clear();
  tetraIndices.clear();

  TetStuffer::stuff(verts, tris, gridSpacing, alphaShort, alphaLong, graded,
                    tetraPoints, tetraIndices);
}

std::vector<Vec3f> &GenericIsosurfaceStuffing::GetTetraVertices() {
  return tetraPoints;
}

std::vector<Tetrahedron> &GenericIsosurfaceStuffing::GetTetras() {
  return tetraIndices;
}
