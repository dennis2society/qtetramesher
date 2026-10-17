/*
 * MeshRepair.cpp
 *
 * See MeshRepair.h
 */

#include "TetraMeshTools/MeshRepair.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <sstream>
#include <tuple>
#include <unordered_map>

namespace {

struct Vec3d {
  double x, y, z;
};

Vec3d toDouble(const Vec3f &v) { return {v.x, v.y, v.z}; }

/// Merges vertices closer than tol, remaps the triangles and returns the
/// number of removed vertices. Uses a uniform grid with cell size tol, so
/// only the 27 neighboring cells have to be searched.
std::size_t weldVertices(std::vector<Vec3f> &verts, std::vector<Triangle> &tris,
                         double tol) {
  struct KeyHash {
    std::size_t operator()(const std::array<std::int64_t, 3> &k) const {
      std::uint64_t h = 1469598103934665603ull;
      for (std::int64_t c : k) h = (h ^ std::uint64_t(c)) * 1099511628211ull;
      return std::size_t(h);
    }
  };
  std::unordered_map<std::array<std::int64_t, 3>, std::vector<std::uint32_t>,
                     KeyHash>
      grid;
  grid.reserve(verts.size());
  const double tol2 = tol * tol;
  const double inv = tol > 0.0 ? 1.0 / tol : 0.0;
  std::vector<Vec3f> welded;
  welded.reserve(verts.size());
  std::vector<std::uint32_t> remap(verts.size());
  for (std::size_t i = 0; i < verts.size(); ++i) {
    const Vec3d p = toDouble(verts[i]);
    const std::array<std::int64_t, 3> cell = {
        std::int64_t(std::floor(p.x * inv)), std::int64_t(std::floor(p.y * inv)),
        std::int64_t(std::floor(p.z * inv))};
    std::int64_t found = -1;
    for (int dx = -1; dx <= 1 && found < 0; ++dx)
      for (int dy = -1; dy <= 1 && found < 0; ++dy)
        for (int dz = -1; dz <= 1 && found < 0; ++dz) {
          auto it = grid.find({cell[0] + dx, cell[1] + dy, cell[2] + dz});
          if (it == grid.end()) continue;
          for (std::uint32_t r : it->second) {
            const Vec3d q = toDouble(welded[r]);
            const double d2 = (p.x - q.x) * (p.x - q.x) +
                              (p.y - q.y) * (p.y - q.y) +
                              (p.z - q.z) * (p.z - q.z);
            if (d2 <= tol2) {
              found = r;
              break;
            }
          }
        }
    if (found < 0) {
      found = std::int64_t(welded.size());
      welded.push_back(verts[i]);
      grid[cell].push_back(std::uint32_t(found));
    }
    remap[i] = std::uint32_t(found);
  }
  for (Triangle &t : tris)
    for (int k = 0; k < 3; ++k) t.index[k] = remap[t.index[k]];
  const std::size_t merged = verts.size() - welded.size();
  verts.swap(welded);
  return merged;
}

/// For every triangle edge k (from corner k to corner k+1): the triangle on
/// the other side if the edge has exactly two triangles, else -1.
/// edgeUses holds the number of triangles sharing each edge.
struct Adjacency {
  std::vector<std::int64_t> neighbor;
  std::vector<std::uint32_t> edgeUses;
};

std::uint64_t edgeKey(std::uint32_t a, std::uint32_t b) {
  if (a > b) std::swap(a, b);
  return (std::uint64_t(a) << 32) | b;
}

Adjacency buildAdjacency(const std::vector<Triangle> &tris) {
  std::vector<std::pair<std::uint64_t, std::uint32_t>> edges;  // key, t*3+k
  edges.reserve(tris.size() * 3);
  for (std::size_t t = 0; t < tris.size(); ++t)
    for (int k = 0; k < 3; ++k)
      edges.emplace_back(
          edgeKey(tris[t].index[k], tris[t].index[(k + 1) % 3]),
          std::uint32_t(t * 3 + k));
  std::sort(edges.begin(), edges.end());
  Adjacency adj;
  adj.neighbor.assign(tris.size() * 3, -1);
  adj.edgeUses.assign(tris.size() * 3, 0);
  for (std::size_t i = 0; i < edges.size();) {
    std::size_t j = i;
    while (j < edges.size() && edges[j].first == edges[i].first) ++j;
    for (std::size_t m = i; m < j; ++m)
      adj.edgeUses[edges[m].second] = std::uint32_t(j - i);
    if (j - i == 2) {
      adj.neighbor[edges[i].second] = edges[i + 1].second / 3;
      adj.neighbor[edges[i + 1].second] = edges[i].second / 3;
    }
    i = j;
  }
  return adj;
}

/// Reverses a triangle. Its edges 0 and 2 swap places (edge 1 stays), so
/// the adjacency entries are swapped as well.
void flip(std::vector<Triangle> &tris, Adjacency &adj, std::size_t t) {
  std::swap(tris[t].index[1], tris[t].index[2]);
  std::swap(adj.neighbor[t * 3], adj.neighbor[t * 3 + 2]);
  std::swap(adj.edgeUses[t * 3], adj.edgeUses[t * 3 + 2]);
}

/// true if triangle u contains the directed edge a->b
bool hasDirectedEdge(const Triangle &u, std::uint32_t a, std::uint32_t b) {
  for (int k = 0; k < 3; ++k)
    if (u.index[k] == a && u.index[(k + 1) % 3] == b) return true;
  return false;
}

/// Connected components over manifold edges.
std::vector<std::vector<std::uint32_t>> components(
    const std::vector<Triangle> &tris, const Adjacency &adj) {
  std::vector<std::vector<std::uint32_t>> result;
  std::vector<char> visited(tris.size(), 0);
  for (std::size_t s = 0; s < tris.size(); ++s) {
    if (visited[s]) continue;
    visited[s] = 1;
    std::vector<std::uint32_t> comp{std::uint32_t(s)};
    for (std::size_t q = 0; q < comp.size(); ++q)
      for (int k = 0; k < 3; ++k) {
        const std::int64_t u = adj.neighbor[comp[q] * 3 + k];
        if (u >= 0 && !visited[u]) {
          visited[u] = 1;
          comp.push_back(std::uint32_t(u));
        }
      }
    result.push_back(std::move(comp));
  }
  return result;
}

/// Makes neighboring triangles traverse their shared edge in opposite
/// directions. Within a component the orientation that needs fewer flips
/// wins. Returns the number of flipped triangles.
std::size_t orientConsistently(std::vector<Triangle> &tris, Adjacency &adj) {
  std::size_t flipped = 0;
  std::vector<char> visited(tris.size(), 0);
  std::vector<char> wasFlipped(tris.size(), 0);
  for (std::size_t s = 0; s < tris.size(); ++s) {
    if (visited[s]) continue;
    visited[s] = 1;
    std::vector<std::uint32_t> comp{std::uint32_t(s)};
    std::size_t compFlips = 0;
    for (std::size_t q = 0; q < comp.size(); ++q) {
      const std::uint32_t t = comp[q];
      for (int k = 0; k < 3; ++k) {
        const std::int64_t u = adj.neighbor[t * 3 + k];
        if (u < 0 || visited[u]) continue;
        visited[u] = 1;
        const std::uint32_t a = tris[t].index[k];
        const std::uint32_t b = tris[t].index[(k + 1) % 3];
        if (hasDirectedEdge(tris[u], a, b)) {
          flip(tris, adj, std::size_t(u));
          wasFlipped[u] = 1;
          ++compFlips;
        }
        comp.push_back(std::uint32_t(u));
      }
    }
    if (compFlips * 2 > comp.size()) {
      // the start triangle was the odd one, flip the whole component instead
      for (std::uint32_t t : comp) flip(tris, adj, t);
      compFlips = 0;
      for (std::uint32_t t : comp) compFlips += wasFlipped[t] ? 0 : 1;
    }
    flipped += compFlips;
  }
  return flipped;
}

/// Closes boundary loops with a fan around their centroid (a single
/// triangle for 3 edges). Loops through vertices with more than one
/// boundary edge pair (pinched) are ambiguous and left open.
void fillHoles(std::vector<Vec3f> &verts, std::vector<Triangle> &tris,
               const Adjacency &adj, MeshRepairReport &report) {
  // boundary edges reversed, so caps get the orientation of their neighbors
  std::unordered_map<std::uint32_t, std::vector<std::uint32_t>> outgoing;
  for (std::size_t t = 0; t < tris.size(); ++t)
    for (int k = 0; k < 3; ++k)
      if (adj.edgeUses[t * 3 + k] == 1)
        outgoing[tris[t].index[(k + 1) % 3]].push_back(tris[t].index[k]);

  std::unordered_map<std::uint32_t, char> used;
  for (const auto &start : outgoing) {
    if (start.second.size() != 1 || used[start.first]) continue;
    std::vector<std::uint32_t> loop{start.first};
    used[start.first] = 1;
    std::uint32_t cur = start.second[0];
    bool ok = true;
    while (cur != start.first) {
      auto it = outgoing.find(cur);
      if (it == outgoing.end() || it->second.size() != 1 || used[cur] ||
          loop.size() > outgoing.size()) {
        ok = false;
        break;
      }
      used[cur] = 1;
      loop.push_back(cur);
      cur = it->second[0];
    }
    if (!ok || loop.size() < 3) continue;
    const std::size_t n = loop.size();
    if (n == 3) {
      Triangle t;
      t.index[0] = loop[0];
      t.index[1] = loop[1];
      t.index[2] = loop[2];
      tris.push_back(t);
    } else {
      double cx = 0, cy = 0, cz = 0;
      for (std::uint32_t v : loop) {
        cx += verts[v].x;
        cy += verts[v].y;
        cz += verts[v].z;
      }
      const std::uint32_t c = std::uint32_t(verts.size());
      verts.push_back(Vec3f(float(cx / n), float(cy / n), float(cz / n)));
      for (std::size_t i = 0; i < n; ++i) {
        Triangle t;
        t.index[0] = loop[i];
        t.index[1] = loop[(i + 1) % n];
        t.index[2] = c;
        tris.push_back(t);
      }
    }
    ++report.numFilledHoles;
    report.numHoleTriangles += n == 3 ? 1 : n;
  }
}

}  // namespace

std::string MeshRepairReport::Summary() const {
  std::ostringstream ss;
  const char *sep = "";
  auto item = [&](std::size_t n, const char *text) {
    if (n == 0) return;
    ss << sep << n << " " << text;
    sep = ", ";
  };
  item(numMergedVertices, "duplicate vertices merged");
  item(numInvalidTriangles, "invalid triangles removed");
  item(numDegenerateTriangles, "degenerate triangles removed");
  item(numDuplicateTriangles, "duplicate triangles removed");
  item(numFlippedTriangles, "triangles flipped");
  item(numInvertedComponents, "inside-out parts turned");
  item(numFilledHoles, "holes filled");
  item(numUnusedVertices, "unused vertices removed");
  return ss.str();
}

MeshRepairReport RepairSurfaceMesh(std::vector<Vec3f> &verts,
                                   std::vector<Triangle> &tris,
                                   bool fillHolesFlag, double tolerance) {
  MeshRepairReport report;

  // triangles with invalid indices or non-finite coordinates
  {
    auto bad = [&](const Triangle &t) {
      for (int k = 0; k < 3; ++k) {
        if (t.index[k] >= verts.size()) return true;
        const Vec3f &v = verts[t.index[k]];
        if (!std::isfinite(v.x) || !std::isfinite(v.y) || !std::isfinite(v.z))
          return true;
      }
      return false;
    };
    const std::size_t before = tris.size();
    tris.erase(std::remove_if(tris.begin(), tris.end(), bad), tris.end());
    report.numInvalidTriangles = before - tris.size();
  }
  if (tris.empty()) return report;

  // merge split and nearly coincident vertices
  {
    Vec3d lo = {1e300, 1e300, 1e300}, hi = {-1e300, -1e300, -1e300};
    for (const Triangle &t : tris)
      for (int k = 0; k < 3; ++k) {
        const Vec3d p = toDouble(verts[t.index[k]]);
        lo = {std::min(lo.x, p.x), std::min(lo.y, p.y), std::min(lo.z, p.z)};
        hi = {std::max(hi.x, p.x), std::max(hi.y, p.y), std::max(hi.z, p.z)};
      }
    const double diag = std::sqrt((hi.x - lo.x) * (hi.x - lo.x) +
                                  (hi.y - lo.y) * (hi.y - lo.y) +
                                  (hi.z - lo.z) * (hi.z - lo.z));
    report.numMergedVertices = weldVertices(verts, tris, tolerance * diag);
  }

  // triangles collapsed by the merge
  {
    const std::size_t before = tris.size();
    tris.erase(std::remove_if(tris.begin(), tris.end(),
                              [](const Triangle &t) {
                                return t.index[0] == t.index[1] ||
                                       t.index[1] == t.index[2] ||
                                       t.index[0] == t.index[2];
                              }),
               tris.end());
    report.numDegenerateTriangles = before - tris.size();
  }

  // duplicate triangles (same corners, any order or orientation)
  {
    std::vector<std::pair<std::array<std::uint32_t, 3>, std::uint32_t>> keys;
    keys.reserve(tris.size());
    for (std::size_t t = 0; t < tris.size(); ++t) {
      std::array<std::uint32_t, 3> k = {tris[t].index[0], tris[t].index[1],
                                        tris[t].index[2]};
      std::sort(k.begin(), k.end());
      keys.emplace_back(k, std::uint32_t(t));
    }
    std::sort(keys.begin(), keys.end());
    std::vector<char> keep(tris.size(), 1);
    for (std::size_t i = 1; i < keys.size(); ++i)
      if (keys[i].first == keys[i - 1].first) keep[keys[i].second] = 0;
    std::vector<Triangle> unique;
    unique.reserve(tris.size());
    for (std::size_t t = 0; t < tris.size(); ++t)
      if (keep[t]) unique.push_back(tris[t]);
    report.numDuplicateTriangles = tris.size() - unique.size();
    tris.swap(unique);
  }

  Adjacency adj = buildAdjacency(tris);
  report.numFlippedTriangles = orientConsistently(tris, adj);

  if (fillHolesFlag) {
    const std::size_t before = tris.size();
    fillHoles(verts, tris, adj, report);
    if (tris.size() != before) adj = buildAdjacency(tris);
  }

  // closed parts with a negative volume are inside out
  for (const std::vector<std::uint32_t> &comp : components(tris, adj)) {
    bool closed = true;
    double volume = 0.0;
    for (std::uint32_t t : comp) {
      for (int k = 0; k < 3; ++k)
        if (adj.neighbor[t * 3 + k] < 0) closed = false;
      const Vec3d a = toDouble(verts[tris[t].index[0]]);
      const Vec3d b = toDouble(verts[tris[t].index[1]]);
      const Vec3d c = toDouble(verts[tris[t].index[2]]);
      volume += a.x * (b.y * c.z - b.z * c.y) - a.y * (b.x * c.z - b.z * c.x) +
                a.z * (b.x * c.y - b.y * c.x);
    }
    if (closed && volume < 0.0) {
      for (std::uint32_t t : comp) flip(tris, adj, t);
      ++report.numInvertedComponents;
    }
  }

  // remove vertices no triangle refers to
  {
    std::vector<std::int64_t> remap(verts.size(), -1);
    std::vector<Vec3f> compact;
    compact.reserve(verts.size());
    for (Triangle &t : tris)
      for (int k = 0; k < 3; ++k) {
        std::int64_t &r = remap[t.index[k]];
        if (r < 0) {
          r = std::int64_t(compact.size());
          compact.push_back(verts[t.index[k]]);
        }
        t.index[k] = unsigned(r);
      }
    report.numUnusedVertices = verts.size() - compact.size();
    verts.swap(compact);
  }

  // what is left (a boundary edge belongs to exactly one triangle)
  report.numBoundaryEdges = std::size_t(
      std::count(adj.edgeUses.begin(), adj.edgeUses.end(), 1u));
  {
    std::vector<std::uint64_t> keys;
    for (std::size_t i = 0; i < adj.edgeUses.size(); ++i)
      if (adj.edgeUses[i] > 2) {
        const Triangle &t = tris[i / 3];
        const int k = int(i % 3);
        keys.push_back(edgeKey(t.index[k], t.index[(k + 1) % 3]));
      }
    std::sort(keys.begin(), keys.end());
    report.numNonManifoldEdges = std::size_t(
        std::unique(keys.begin(), keys.end()) - keys.begin());
  }
  return report;
}
