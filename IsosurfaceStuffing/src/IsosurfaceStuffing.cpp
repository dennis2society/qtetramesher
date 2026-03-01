/*
 * IsosurfaceStuffing.cpp
 *
 * Wrapper that delegates to the TetStuffer pipeline.
 *
 *  Created 2026
 */

#include "IsosurfaceStuffing.h"
#include "TetStuffer.h"

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

void IsosurfaceStuffing::GenerateFromSurface(
    const std::vector<Triangle> &tris,
    const std::vector<Vec3f> &verts,
    float gridSpacing,
    float alphaShort,
    float alphaLong) {

  tetraPoints.clear();
  tetraIndices.clear();

  TetStuffer::stuff(verts, tris, gridSpacing, alphaShort, alphaLong,
                    tetraPoints, tetraIndices);
}

std::vector<Vec3f> &IsosurfaceStuffing::GetTetraVertices() {
  return tetraPoints;
}

std::vector<Tetrahedron> &IsosurfaceStuffing::GetTetras() {
  return tetraIndices;
}
