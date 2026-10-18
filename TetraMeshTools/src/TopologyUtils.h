/*
 * TopologyUtils.h
 *
 * Internal helpers for TriangleTopology and TetrahedronTopology.
 *
 * The topology generation used to rely on std::map lookups and on loops
 * over all primitives for every vertex (O(vertices * primitives)). These
 * helpers provide the same results with sorting, binary search and counting
 * sort, which is orders of magnitude faster for large meshes.
 */

#ifndef TETRAMESHTOOLS_TOPOLOGYUTILS_H_
#define TETRAMESHTOOLS_TOPOLOGYUTILS_H_

#include <algorithm>
#include <cstddef>
#include <limits>
#include <numeric>
#include <vector>

#include "TriangleTopology.h"

namespace TetraTools {
namespace detail {

const unsigned int kNotFound = std::numeric_limits<unsigned int>::max();

/// Finds the index of a primitive (Edge, Triangle) in a vector. Uses binary
/// search directly on the vector if it is sorted (the usual case), otherwise
/// on a sorted index list.
template <typename T>
class IndexFinder {
 public:
  explicit IndexFinder(const std::vector<T> &data_) : data(data_) {
    sorted = std::is_sorted(data.begin(), data.end());
    if (!sorted) {
      order.resize(data.size());
      std::iota(order.begin(), order.end(), 0u);
      std::stable_sort(order.begin(), order.end(),
                       [this](unsigned int a, unsigned int b) {
                         return data[a] < data[b];
                       });
    }
  }

  /// returns kNotFound if the primitive does not exist
  unsigned int find(const T &value) const {
    if (sorted) {
      auto it = std::lower_bound(data.begin(), data.end(), value);
      if (it == data.end() || value < *it) return kNotFound;
      return static_cast<unsigned int>(it - data.begin());
    }
    auto it = std::lower_bound(
        order.begin(), order.end(), value,
        [this](unsigned int a, const T &v) { return data[a] < v; });
    if (it == order.end() || value < data[*it]) return kNotFound;
    return *it;
  }

 private:
  const std::vector<T> &data;
  std::vector<unsigned int> order;
  bool sorted;
};

/// Builds the "primitives per vertex" lists with a counting sort: for every
/// vertex all (primitive index, position in primitive) pairs, ordered by
/// primitive index and position, plus the offset/length lookup table.
/// N is the number of vertex indices per primitive, makeEntry(j, k) creates
/// the entry for position k of primitive j. Indices outside the vertex range
/// are ignored. maxCount/maxVertex receive the largest list (the last vertex
/// in case of a tie).
template <unsigned int N, typename Prim, typename Entry, typename MakeEntry>
void BuildVertexMap(std::size_t numVertices, const std::vector<Prim> &prims,
                    std::vector<Entry> &entries,
                    std::vector<PrimitivesPerVertex> &lookup,
                    MakeEntry makeEntry, unsigned int &maxCount,
                    unsigned int &maxVertex) {
  std::vector<unsigned int> count(numVertices, 0);
  for (const Prim &p : prims) {
    for (unsigned int k = 0; k < N; ++k) {
      if (p.index[k] < numVertices) ++count[p.index[k]];
    }
  }
  lookup.resize(numVertices);
  unsigned int offset = 0;
  maxCount = 0;
  maxVertex = 0;
  for (std::size_t i = 0; i < numVertices; ++i) {
    lookup[i].offset = offset;
    lookup[i].length = count[i];
    offset += count[i];
    if (count[i] >= maxCount) {
      maxCount = count[i];
      maxVertex = static_cast<unsigned int>(i);
    }
  }
  entries.resize(offset);
  // reuse count as the insert position of every vertex
  for (std::size_t i = 0; i < numVertices; ++i) count[i] = lookup[i].offset;
  for (std::size_t j = 0; j < prims.size(); ++j) {
    for (unsigned int k = 0; k < N; ++k) {
      const unsigned int v = prims[j].index[k];
      if (v < numVertices)
        entries[count[v]++] = makeEntry(static_cast<unsigned int>(j), k);
    }
  }
}

}  // namespace detail
}  // namespace TetraTools

#endif /* TETRAMESHTOOLS_TOPOLOGYUTILS_H_ */
