/*
 * TriangleTopology.cpp
 *
 *  Created on: Aug 20, 2011
 *      Author: Dennis Luebke
 */

#include "TriangleTopology.h"
#include "TopologyUtils.h"
#include <algorithm>
#include <iostream>
#include <limits>
#include <map>
#ifndef WIN32
#include <cfloat>
#include <math.h>
#else
#include <float.h>
#endif

/// Constructors
TetraTools::TriangleTopology::TriangleTopology() : radius(0) { Clear(); }

TetraTools::TriangleTopology::TriangleTopology(
    const std::vector<Vec3f> &vertices_,
    const std::vector<Triangle> &triangles_, const bool complete_)
    : radius(0) {
  Init(vertices_, triangles_, complete_);
}

TetraTools::TriangleTopology::~TriangleTopology() { Clear(); }

void TetraTools::TriangleTopology::GenerateNormals() {
  _normals.clear();
  _normals.resize(_vertices.size());
  for (auto tri = _triangles.begin(); tri != _triangles.end(); ++tri) {
    Vec3f e1 = _vertices[tri->index[1]] - _vertices[tri->index[0]];
    Vec3f e2 = _vertices[tri->index[2]] - _vertices[tri->index[0]];
    Vec3f normal = e1.cross(e2);
    normal.normalize();

    _normals[tri->index[0]] += normal;
    _normals[tri->index[1]] += normal;
    _normals[tri->index[2]] += normal;
  }
  for (auto n = _normals.begin(); n != _normals.end(); ++n) {
    n->normalize();
  }
}

void TetraTools::TriangleTopology::GenerateEdges() {
  std::cout << "Generating Edges..." << std::endl;
  if (_triangles.size() == 0) {
    std::cerr << "ERROR! Cannot generate Edges. No Triangles present!"
              << std::endl;
    return;
  }
  // collect all triangle edges (smaller index first), then sort and remove
  // duplicates; this gives the sorted list of unique edges
  _edges.clear();
  _edges.reserve(_triangles.size() * 3);
  for (const Triangle &t : _triangles) {
    for (unsigned int j = 0; j < 3; ++j) {
      const unsigned int v1 = t.index[j];
      const unsigned int v2 = t.index[(j < 2) ? (j + 1) : 0];
      Edge e;
      e.Set(std::min(v1, v2), std::max(v1, v2));
      _edges.push_back(e);
    }
  }
  std::sort(_edges.begin(), _edges.end());
  _edges.erase(std::unique(_edges.begin(), _edges.end(),
                           [](const Edge &a, const Edge &b) {
                             return !(a < b) && !(b < a);
                           }),
               _edges.end());
  _edges.shrink_to_fit();
  std::cout << "\tNumEdges: " << _edges.size() << std::endl;
}

void TetraTools::TriangleTopology::GenerateTriangleEdges() {
  std::cout << "Generating TriangleEdges..." << std::endl;
  if (_triangles.size() == 0) {
    std::cerr << "ERROR! Cannot generate TriangleEdges. No Triangles present!"
              << std::endl;
    return;
  }
  _triangleEdges.clear();
  _triangleEdges.resize(_triangles.size());
  const detail::IndexFinder<Edge> edges(GetEdges());
  for (std::size_t i = 0; i < _triangles.size(); ++i) {
    const Triangle &t = _triangles[i];
    TriangleEdges &te = _triangleEdges[i];
    for (unsigned int ti = 0; ti < 3; ++ti) {
      const unsigned int a = t.index[ti];
      const unsigned int b = t.index[(ti < 2) ? (ti + 1) : 0];
      Edge e;
      e.Set(std::min(a, b), std::max(a, b));
      te.index[ti] = edges.find(e);
    }
  }
}

void TetraTools::TriangleTopology::GenerateEdgeMap() {
  std::cout << "Generating EdgeMap..." << std::endl;
  if (_edges.size() == 0) {
    std::cerr << "ERROR! Cannot generate EdgeMap. No Edges present!"
              << std::endl;
    return;
  }
  unsigned int maxEdges = 0, edgesMax = 0;
  detail::BuildVertexMap<2>(
      _vertices.size(), _edges, _edgeVertices, _vertexEdgesLookup,
      [](unsigned int edge, unsigned int position) {
        EdgeVertex ev;
        ev.edgeIndex = edge;
        ev.indexInEdge = position;
        return ev;
      },
      maxEdges, edgesMax);
}

void TetraTools::TriangleTopology::GenerateTriangleMap() {
  std::cout << "Generating TriangleMap..." << std::endl;
  unsigned int maxTriangles = 0, trianglesMax = 0;
  detail::BuildVertexMap<3>(
      _vertices.size(), _triangles, _triangleVertices, _vertexTrianglesLookup,
      [](unsigned int triangle, unsigned int position) {
        TriangleVertex tv;
        tv.triangleIndex = triangle;
        tv.indexInTriangle = position;
        return tv;
      },
      maxTriangles, trianglesMax);
  std::cout << "\tMax number of triangles connected to a vertex: "
            << maxTriangles << " at Vertex: " << trianglesMax << std::endl;
}

void TetraTools::TriangleTopology::GenerateBoundingBox() {
  GenerateBoundingBox(_vertices);
}

void TetraTools::TriangleTopology::GenerateBoundingBox(
    const std::vector<Vec3f> &vertices_) {
  /// generate Bounding Box
  std::cout << "Generating BoundingBox ..." << std::endl;
  BoundingBox b;
  // b.min = Vec3f(FLT_MAX, FLT_MAX, FLT_MAX);
  b.min = Vec3f(std::numeric_limits<float>::max(),
                std::numeric_limits<float>::max(),
                std::numeric_limits<float>::max());
  // b.max = Vec3f(FLT_MIN, FLT_MIN, FLT_MIN);
  b.max = Vec3f(-std::numeric_limits<float>::max(),
                -std::numeric_limits<float>::max(),
                -std::numeric_limits<float>::max());
  float dist = 0;
  for (auto it = vertices_.begin(); it != vertices_.end(); ++it) {
    Vec3f v = *it;
    float pd = v.squaredLength();
    if (pd > dist) {
      dist = pd;
    }
    //_vertices.push_back(v);
    b.min.x = std::min<float>(b.min.x, v.x);
    b.max.x = std::max<float>(b.max.x, v.x);
    b.min.y = std::min<float>(b.min.y, v.y);
    b.max.y = std::max<float>(b.max.y, v.y);
    b.min.z = std::min<float>(b.min.z, v.z);
    b.max.z = std::max<float>(b.max.z, v.z);
  }
  bb = b;
  float dx = b.max.x - b.min.x;
  float dy = b.max.y - b.min.y;
  float dz = b.max.z - b.min.z;
  float maxDist = (dx > dy) ? dx : dy;
  maxDist = (maxDist > dz) ? maxDist : dz;
  bc.size = maxDist;
  bc.center = Vec3f(b.min.x + dx / 2, b.min.y + dy / 2, b.min.z + dz / 2);
  const float lbcModifier =
      maxDist * 0.1f; /// add 10% volume for the larger bounding cube
  lbc.size = maxDist + lbcModifier;
  lbc.center = Vec3f(b.min.x + dx / 2, b.min.y + dy / 2, b.min.z + dz / 2);
  // std::cout<<"dx/dy/dz: "<<dx<<"/"<<dy<<"/"<<dz<<"; bc:
  // "<<maxDist<<std::endl;
  std::cout << "\t BBox: " << bb.min << "; " << bb.max << std::endl;
}

void TetraTools::TriangleTopology::Init(const std::vector<Vec3f> &vertices_,
                                        const std::vector<Triangle> &triangles_,
                                        const bool fullUpdate_) {
  Clear();
  _vertices = vertices_;
  GenerateBoundingBox();
  _triangles = triangles_;
}

void TetraTools::TriangleTopology::Clear() {
  _vertices.clear();
  _triangles.clear();
  _edges.clear();
  _triangleEdges.clear();
  _triangleVertices.clear();
  _edgeVertices.clear();
  _vertexEdgesLookup.clear();
  _vertexTrianglesLookup.clear();
}

void TetraTools::TriangleTopology::SetEdges(const std::vector<Edge> &edges_) {
  _edges.clear();
  for (auto i = 0; i < edges_.size(); ++i) {
    _edges.push_back(edges_[i]);
  }
  _edgeVertices.clear();
  _vertexEdgesLookup.clear();
  _triangleEdges.clear();
}

unsigned int
TetraTools::TriangleTopology::FindEdgeByIndex(const unsigned int i0,
                                              const unsigned int i1) {
  if (_edges.size() == 0) {
    GenerateEdges();
    if (_edges.size() == 0)
      return -1;
  }
  for (auto i = 0; i < _edges.size(); ++i) {
    const Edge e = _edges[i];
    if (((e.index[0] == i0) && (e.index[1] == i1)) ||
        ((e.index[0] == i1) && (e.index[1] == i0))) {
      return i;
    }
  }
  return -1;
}
