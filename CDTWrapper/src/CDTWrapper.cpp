/*
 * CDTWrapper.cpp
 *
 * Wrapper for the constrained Delaunay tetrahedrization (CDT) by Diazzi,
 * Panozzo, Vaxman and Attene. https://github.com/MarcoAttene/CDT
 */

#ifdef _MSC_VER
// https://developercommunity.visualstudio.com/t/error-c2872-byte-ambiguous-symbol/93889
#define _HAS_STD_BYTE 0
#endif

// The CDT headers define non-inline functions, so only this file includes them.
#include "cdt.h"

#include <CDTWrapper.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

namespace {

const std::size_t kMaxInteriorPoints = 2000000;

double coord(const Vec3f &v, int d) { return d == 0 ? v.x : (d == 1 ? v.y : v.z); }

/// Regular grid of points in the bounding box that keep a distance of at least
/// half the spacing to the bounding boxes of the triangles.
void addInteriorPoints(const std::vector<Triangle> &tris,
                       const std::vector<Vec3f> &verts, double spacing,
                       std::vector<double> &coords) {
  double bbMin[3] = {verts[0].x, verts[0].y, verts[0].z};
  double bbMax[3] = {bbMin[0], bbMin[1], bbMin[2]};
  for (const Vec3f &v : verts) {
    for (int d = 0; d < 3; ++d) {
      bbMin[d] = std::min(bbMin[d], coord(v, d));
      bbMax[d] = std::max(bbMax[d], coord(v, d));
    }
  }
  std::size_t n[3];
  double cells = 1.0;
  for (int d = 0; d < 3; ++d) {
    n[d] = std::size_t(std::max(1.0, std::ceil((bbMax[d] - bbMin[d]) / spacing)));
    cells *= double(n[d]);
  }
  if (cells > double(kMaxInteriorPoints) * 4.0)
    throw std::runtime_error(
        "CDT: the interior point spacing is too small for this surface "
        "(too many points). Increase the spacing.");

  auto cellRange = [&](double lo, double hi, int d, std::size_t &first,
                       std::size_t &last) {
    const double a = std::floor((lo - bbMin[d]) / spacing);
    const double b = std::floor((hi - bbMin[d]) / spacing);
    first = std::size_t(std::clamp(a, 0.0, double(n[d] - 1)));
    last = std::size_t(std::clamp(b, 0.0, double(n[d] - 1)));
  };
  std::vector<char> nearSurface(n[0] * n[1] * n[2], 0);
  const double margin = 0.5 * spacing;
  for (const Triangle &t : tris) {
    std::size_t first[3], last[3];
    for (int d = 0; d < 3; ++d) {
      double lo = coord(verts[t.index[0]], d), hi = lo;
      for (int k = 1; k < 3; ++k) {
        lo = std::min(lo, coord(verts[t.index[k]], d));
        hi = std::max(hi, coord(verts[t.index[k]], d));
      }
      cellRange(lo - margin, hi + margin, d, first[d], last[d]);
    }
    for (std::size_t i = first[0]; i <= last[0]; ++i)
      for (std::size_t j = first[1]; j <= last[1]; ++j)
        for (std::size_t k = first[2]; k <= last[2]; ++k)
          nearSurface[(i * n[1] + j) * n[2] + k] = 1;
  }

  std::size_t added = 0;
  for (std::size_t i = 0; i < n[0]; ++i)
    for (std::size_t j = 0; j < n[1]; ++j)
      for (std::size_t k = 0; k < n[2]; ++k) {
        if (nearSurface[(i * n[1] + j) * n[2] + k]) continue;
        if (++added > kMaxInteriorPoints)
          throw std::runtime_error(
              "CDT: the interior point spacing is too small for this surface "
              "(too many points). Increase the spacing.");
        coords.push_back(bbMin[0] + (double(i) + 0.5) * spacing);
        coords.push_back(bbMin[1] + (double(j) + 0.5) * spacing);
        coords.push_back(bbMin[2] + (double(k) + 0.5) * spacing);
      }
  std::cout << "CDTWrapper: added " << added << " interior points (spacing "
            << spacing << ")." << std::endl;
}

}  // namespace

void CDTWrapper::GenerateFromSurface(const std::vector<Triangle> &tris,
                                     const std::vector<Vec3f> &verts,
                                     bool innerOnly, bool boundingBox,
                                     double interiorSpacing, bool verbose) {
  tetraPoints.clear();
  tetraIndices.clear();
  if (verts.empty() || tris.empty())
    throw std::runtime_error("CDT: the surface mesh is empty.");

  std::vector<uint32_t> triIndices;
  triIndices.reserve(tris.size() * 3);
  for (const Triangle &t : tris)
    for (int k = 0; k < 3; ++k) {
      if (t.index[k] >= verts.size())
        throw std::runtime_error(
            "CDT: the surface mesh has invalid vertex indices.");
      triIndices.push_back(uint32_t(t.index[k]));
    }
  std::vector<double> coords;
  coords.reserve(verts.size() * 3);
  for (const Vec3f &v : verts) {
    coords.push_back(v.x);
    coords.push_back(v.y);
    coords.push_back(v.z);
  }
  if (interiorSpacing > 0.0)
    addInteriorPoints(tris, verts, interiorSpacing, coords);

  try {
    // removes duplicate vertices and triangles, fails on degenerate triangles
    inputPLC plc;
    plc.initFromVectors(coords.data(), uint32_t(coords.size() / 3),
                        triIndices.data(), uint32_t(tris.size()), verbose);
    if (boundingBox) plc.addBoundingBoxVertices();

    // same steps as CDT::createSteinerCDT(), which hides is_polyhedron
    std::unique_ptr<TetMesh> tin(new TetMesh);
    tin->init_vertices(plc.coordinates.data(), plc.numVertices());
    tin->tetrahedrize();
    if (verbose) std::cout << "CDTWrapper: Delaunay tetrahedrization built." << std::endl;

    PLCx steinerPLC(*tin, plc.triangle_vertices.data(), plc.numTriangles());
    steinerPLC.segmentRecovery_HSi(!verbose);
    steinerPLC.faceRecovery(!verbose);
    const std::size_t numInner = steinerPLC.markInnerTets();

    std::cout << "CDTWrapper: " << steinerPLC.numSteinerVertices()
              << " Steiner points inserted." << std::endl;
    if (innerOnly && !steinerPLC.is_polyhedron)
      std::cout << "CDTWrapper: the surface is not closed, all tetrahedra of "
                   "the convex hull are kept." << std::endl;

    std::vector<uint32_t> remap(tin->numVertices(), UINT32_MAX);
    tetraIndices.reserve(innerOnly ? numInner : tin->countNonGhostTets());
    for (uint32_t t = 0; t < tin->numTets(); ++t) {
      if (tin->isGhost(t)) continue;
      if (innerOnly && tin->mark_tetrahedra[t] != DT_IN) continue;
      Tetrahedron tet;
      for (int k = 0; k < 4; ++k) {
        const uint32_t v = tin->tet_node[t * 4 + k];
        if (remap[v] == UINT32_MAX) {
          double x, y, z;
          tin->getVertexCoordinates(v, x, y, z);
          remap[v] = uint32_t(tetraPoints.size());
          tetraPoints.push_back(Vec3f(float(x), float(y), float(z)));
        }
        tet.index[k] = remap[v];
      }
      // CDT orients all tetrahedra negatively
      std::swap(tet.index[2], tet.index[3]);
      tetraIndices.push_back(tet);
    }

    size_t numFlipped = 0, numFlat = 0;
    tin->hasBadSnappedOrientations(numFlipped, numFlat);
    if (numFlipped > 0 || numFlat > 0)
      std::cout << "CDTWrapper: after rounding the Steiner points, "
                << numFlipped << " tetrahedra are inverted and " << numFlat
                << " are flat." << std::endl;
  } catch (const std::runtime_error &e) {
    std::string message = e.what();
    while (!message.empty() && message.back() == '\n') message.pop_back();
    throw std::runtime_error("CDT: " + message);
  }

  if (tetraIndices.empty())
    throw std::runtime_error("CDT did not generate any tetrahedra.");
  std::cout << "CDTWrapper: " << tetraPoints.size() << " vertices, "
            << tetraIndices.size() << " tetrahedra." << std::endl;
}
