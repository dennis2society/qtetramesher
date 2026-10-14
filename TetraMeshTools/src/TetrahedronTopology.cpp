/*
 * TetrahedronTopology.cpp
 *
 *  Created on: Aug 20, 2011
 *      Author: Dennis Luebke
 */

#include "TetrahedronTopology.h"

#include <algorithm>
#include <cstdint>

#include "TopologyUtils.h"
#ifndef WIN32
#include <math.h>

#include <cfloat>
#else
#include <float.h>
#endif


namespace {

/// Face f of a tetrahedron in the orientation used throughout this class:
/// rotated so that the smallest index comes first; tri has the orientation
/// (v0, v2, v1), rTri the reverse one (v0, v1, v2).
void TetraFace(const Tetrahedron &t, unsigned int f, Triangle &tri,
               Triangle &rTri) {
  unsigned int v[3], val;
  if (f % 2) {
    v[0] = t.index[(f + 1) % 4];
    v[1] = t.index[(f + 2) % 4];
    v[2] = t.index[(f + 3) % 4];
  } else {
    v[0] = t.index[(f + 1) % 4];
    v[2] = t.index[(f + 2) % 4];
    v[1] = t.index[(f + 3) % 4];
  }
  // rotate v such that v[0] is the smallest one
  while ((v[0] > v[1]) || (v[0] > v[2])) {
    val = v[0];
    v[0] = v[1];
    v[1] = v[2];
    v[2] = val;
  }
  tri.index[0] = v[0];
  tri.index[1] = v[2];
  tri.index[2] = v[1];
  rTri.index[0] = v[0];
  rTri.index[1] = v[1];
  rTri.index[2] = v[2];
}

/// all tetrahedron faces, identified by their (sorted) vertex set
struct FaceRef {
  unsigned int key[3];  // sorted vertex indices
  std::uint64_t face;   // 4 * tetrahedron index + face index

  bool operator<(const FaceRef &o) const {
    for (int i = 0; i < 3; ++i) {
      if (key[i] != o.key[i]) return key[i] < o.key[i];
    }
    return face < o.face;
  }
  bool SameKey(const FaceRef &o) const {
    return key[0] == o.key[0] && key[1] == o.key[1] && key[2] == o.key[2];
  }
};

/// Sorted list of all tetrahedron faces. Faces shared by several tetrahedra
/// are adjacent, in the order of their occurrence.
std::vector<FaceRef> SortedFaces(const std::vector<Tetrahedron> &tetras) {
  std::vector<FaceRef> faces(tetras.size() * 4);
  Triangle tri, rTri;
  for (std::size_t i = 0; i < tetras.size(); ++i) {
    for (unsigned int f = 0; f < 4; ++f) {
      TetraFace(tetras[i], f, tri, rTri);
      FaceRef &r = faces[i * 4 + f];
      r.key[0] = tri.index[0];
      r.key[1] = std::min(tri.index[1], tri.index[2]);
      r.key[2] = std::max(tri.index[1], tri.index[2]);
      r.face = i * 4 + f;
    }
  }
  std::sort(faces.begin(), faces.end());
  return faces;
}

/// the triangle (orientation tri) of a face reference
Triangle FaceTriangle(const std::vector<Tetrahedron> &tetras,
                      std::uint64_t face) {
  Triangle tri, rTri;
  TetraFace(tetras[face / 4], static_cast<unsigned int>(face % 4), tri, rTri);
  return tri;
}

}  // namespace

TetraTools::TetrahedronTopology::TetrahedronTopology() { Clear(); }

TetraTools::TetrahedronTopology::TetrahedronTopology(
    const std::vector<Vec3f> &vertices_,
    const std::vector<Tetrahedron> &tetras_, const bool complete_) {
  Clear();
  Init(vertices_, tetras_, complete_);
}

TetraTools::TetrahedronTopology::~TetrahedronTopology() { Clear(); }

void TetraTools::TetrahedronTopology::Init(
    const std::vector<Vec3f> &vertices_,
    const std::vector<Tetrahedron> &tetras_, const bool complete_) {
  /// generate Bounding Box
  Clear();
  _vertices = vertices_;
  GenerateBoundingBox();
  std::cout << "Num Tetras: " << tetras_.size() << std::endl;
  _tetrahedra = tetras_;
  GenerateTriangles();
  GenerateEdges();
  if (complete_) {
    GenerateEdgeMap();
    GenerateTetraEdges();
    GenerateTetraTriangles();
    GenerateTetrahedronMap();
    GenerateTriangleEdges();
    GenerateTriangleMap();
    GenerateSurfaceTriangles();
  }
}

void TetraTools::TetrahedronTopology::Clear() {
  _vertices.clear();
  _triangles.clear();
  _edges.clear();
  _triangleEdges.clear();
  _triangleVertices.clear();
  _edgeVertices.clear();
  _vertexEdgesLookup.clear();
  _vertexTrianglesLookup.clear();
  _tetrahedra.clear();
  _tetraEdges.clear();
  _tetraVertices.clear();
  _vertexTetrahedraLookup.clear();
  _surfaceTriangles.clear();
  _tetraTriangles.clear();
}

void TetraTools::TetrahedronTopology::GenerateTriangles() {
  std::cout << "Generating Triangles from Tetrahedra..." << std::endl;
  // one triangle per distinct face, in the orientation of its first
  // occurrence
  const std::vector<FaceRef> faces = SortedFaces(_tetrahedra);
  _triangles.clear();
  for (std::size_t i = 0; i < faces.size(); ++i) {
    if (i == 0 || !faces[i].SameKey(faces[i - 1]))
      _triangles.push_back(FaceTriangle(_tetrahedra, faces[i].face));
  }
  _triangles.shrink_to_fit();
  std::cout << "\tNum Triangles in TetraMesh: " << _triangles.size()
            << std::endl;
  std::sort(_triangles.begin(), _triangles.end());
}

void TetraTools::TetrahedronTopology::GenerateTetraEdges() {
  std::cout << "Generating TetraEdges..." << std::endl;
  _tetraEdges.clear();
  _tetraEdges.resize(_tetrahedra.size());
  const detail::IndexFinder<Edge> edges(GetEdges());
  // the six edges in the order (0,1) (0,2) (0,3) (1,2) (1,3) (2,3)
  static const unsigned int pairs[6][2] = {{0, 1}, {0, 2}, {0, 3},
                                           {1, 2}, {1, 3}, {2, 3}};
  Edge tmpEdge;
  for (std::size_t i = 0; i < _tetrahedra.size(); ++i) {
    const Tetrahedron &t = _tetrahedra[i];
    for (unsigned int k = 0; k < 6; ++k) {
      tmpEdge.index[0] = t.index[pairs[k][0]];
      tmpEdge.index[1] = t.index[pairs[k][1]];
      OrderEdge(tmpEdge);
      _tetraEdges[i].index[k] = edges.find(tmpEdge);
    }
  }
}

void TetraTools::TetrahedronTopology::GenerateTetrahedronMap() {
  std::cout << "Generating TetraMap..." << std::endl;
  unsigned int maxTetras = 0, tetrasMax = 0;
  detail::BuildVertexMap<4>(
      _vertices.size(), _tetrahedra, _tetraVertices, _vertexTetrahedraLookup,
      [](unsigned int tetra, unsigned int position) {
        TetrahedronVertex tv;
        tv.tetraIndex = tetra;
        tv.indexInTetra = position;
        return tv;
      },
      maxTetras, tetrasMax);
  std::cout << "\tMax number of tetrahedra connected to a vertex: " << maxTetras
            << " at Vertex: " << tetrasMax << std::endl;
}

void TetraTools::TetrahedronTopology::GenerateSurfaceTriangles() {
  std::cout << "Generating SurfaceTriangles from tetrahedra..." << std::endl;
  // surface triangles are the faces that belong to exactly one tetrahedron
  const std::vector<FaceRef> faces = SortedFaces(_tetrahedra);
  _surfaceTriangles.clear();
  for (std::size_t i = 0; i < faces.size();) {
    std::size_t j = i + 1;
    while (j < faces.size() && faces[j].SameKey(faces[i])) ++j;
    if (j - i == 1)
      _surfaceTriangles.push_back(FaceTriangle(_tetrahedra, faces[i].face));
    i = j;
  }
  std::sort(_surfaceTriangles.begin(), _surfaceTriangles.end());
  std::cout << "\tNum surface triangles: " << _surfaceTriangles.size()
            << std::endl;
}

void TetraTools::TetrahedronTopology::GenerateTetraTriangles() {
  std::cout << "Generating triangles per tetrahedron..." << std::endl;
  _tetraTriangles.clear();
  _tetraTriangles.resize(_tetrahedra.size());
  const detail::IndexFinder<Triangle> triangles(_triangles);
  Triangle tri, rTri;
  for (std::size_t k = 0; k < _tetrahedra.size(); ++k) {
    for (unsigned int f = 0; f < 4; ++f) {
      TetraFace(_tetrahedra[k], f, tri, rTri);
      // the triangle exists in one of the two orientations
      unsigned int index = triangles.find(tri);
      if (index == detail::kNotFound) index = triangles.find(rTri);
      _tetraTriangles[k].index[f] = index;
    }
  }
}
