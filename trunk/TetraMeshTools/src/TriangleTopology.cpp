/*
 * TriangleTopology.cpp
 *
 *  Created on: Aug 20, 2011
 *      Author: Dennis Luebke
 */

#include "TriangleTopology.h"
#include <map>
#include <iostream>
#include <algorithm>
#include <limits>
#ifndef WIN32
#include <cfloat>
#include <math.h>
#else
#include <float.h>
#endif

/// Constructors
TetraTools::TriangleTopology::TriangleTopology() : radius(0)
{
	Clear();
}

TetraTools::TriangleTopology::TriangleTopology(	const std::vector<Vec3f>& vertices_,
												const std::vector<Triangle>& triangles_,
												const bool complete_) : radius(0)
{
	Init(vertices_, triangles_, complete_);
}

TetraTools::TriangleTopology::~TriangleTopology()
{
	Clear();
}

void TetraTools::TriangleTopology::GenerateNormals()
{
	_normals.clear();
	if (_vertexTrianglesLookup.size() != _vertices.size())
	{
		GenerateTriangleMap();
	}
	std::cout<<"Generating Surface Normals..."<<std::endl;
	float dist = 0;
	for (unsigned int i=0; i<_vertices.size(); ++i)
	{
		Vec3f normal;
		PrimitivesPerVertex ppv = _vertexTrianglesLookup[i];
		for (unsigned int j=ppv.offset; j<ppv.length+ppv.offset; ++j)
		{
			TriangleVertex tv = _triangleVertices[j];
			Triangle t = _triangles[tv.triangleIndex];
			normal += t.GetNormal(_vertices[t.index[0]], _vertices[t.index[1]], _vertices[t.index[2]]);
		}
		normal.normalize();
		_normals.push_back(normal);
	}
	radius = sqrt(dist);
}

void TetraTools::TriangleTopology::GenerateEdges()
{
	std::cout<<"Generating Edges..."<<std::endl;
	if (_triangles.size() == 0)
	{
		std::cerr<<"ERROR! Cannot generate Edges. No Triangles present!"<<std::endl;
		return;
	}
	if (_edges.size() != 0)
		_edges.clear();
	// create a temporary map to find redundant edges
	std::map<Edge,unsigned int> edgeMap;
	std::map<Edge,unsigned int>::iterator itt;
	for (unsigned int i = 0; i < _triangles.size(); ++i) {
		const Triangle& t = _triangles[i];
		for (unsigned int j=0; j<3; ++j) {
			const unsigned int v1 = t.index[j];
			const unsigned int v2 = t.index[(j<2)?(j+1):0];
			// validate that the smaller index is the first one, otherwise swap them.
			Edge e;
			if (v1<v2)
			{
				e.index[0] = v1;
				e.index[1] = v2;
			}
			else
			{
				e.index[0] = v2;
				e.index[1] = v1;
			}
			// determine whether the current edge already exists, if not: add it
			if (edgeMap.find(e) == edgeMap.end()) {
				// edge not in edgeMap so create a new one
				const int edgeIndex = edgeMap.size();
				edgeMap[e] = edgeIndex;
				_edges.push_back(e);
			}
		}
	}
	/// sort edge list from smallest to largest first index
	std::sort(_edges.begin(), _edges.end());
	std::cout<<"\tNumEdges: "<<_edges.size()<<std::endl;
}

void TetraTools::TriangleTopology::GenerateTriangleEdges()
{
	std::cout<<"Generating TriangleEdges..."<<std::endl;
	if (_triangles.size() == 0)
	{
		std::cerr<<"ERROR! Cannot generate TriangleEdges. No Triangles present!"<<std::endl;
		return;
	}
	if (_triangleEdges.size() != 0)
		_triangleEdges.clear();
	/// generate Edge-To-Indices map for faster lookup
	std::map<Edge, unsigned int> m;
	for (unsigned int i=0; i<_edges.size(); ++i)
	{
		const Edge& e = _edges[i];
		m[e] = i;
	}
	std::map<Edge, unsigned int>::iterator it;
	Edge e[3];
	for (unsigned int i=0; i<_triangles.size(); ++i)
	{
		const Triangle& t = _triangles[i];
		TriangleEdges te;
		for (unsigned int ti=0; ti<3; ++ti)
		{
			/// eventually swap edge-indices
			if (ti<2) {
				if (t.index[ti+1] > t.index[ti])
					e[ti].Set(t.index[ti], t.index[ti+1]);
				else
					e[ti].Set(t.index[ti+1], t.index[ti]);
			}
			else
			{
				if (t.index[0] > t.index[2])
					e[ti].Set(t.index[2], t.index[0]);
				else
					e[ti].Set(t.index[0], t.index[2]);
			}
			it = m.find(e[ti]);
			te.index[ti] = it->second;
		}
		_triangleEdges.push_back(te);
	}
}

void TetraTools::TriangleTopology::GenerateEdgeMap()
{
	std::cout<<"Generating EdgeMap..."<<std::endl;
	if (_edges.size() == 0)
	{
		std::cerr<<"ERROR! Cannot generate EdgeMap. No Edges present!"<<std::endl;
		return;
	}
	unsigned int numVerts = _vertices.size();
	unsigned int numEdges = _edges.size();
	unsigned int offset = 0;
	unsigned int maxEdges = 0;
	unsigned int edgeMax = 0;
	for (unsigned int i=0; i<numVerts; ++i) {
		PrimitivesPerVertex veMap;
		unsigned int counter = 0;
		veMap.offset = offset;
		for (unsigned int j=0; j<numEdges; ++j) {
			const Edge& e = _edges[j];
			unsigned int vertInEdge = 0;
			EdgeVertex edgeVertex;
			if (e.index[0] == i) {
				edgeVertex.edgeIndex = j;
				edgeVertex.indexInEdge = vertInEdge;
				++counter;
				++offset;
				_edgeVertices.push_back(edgeVertex);
			}
			++vertInEdge;
			if (e.index[1] == i) {
				edgeVertex.edgeIndex = j;
				edgeVertex.indexInEdge = vertInEdge;
				++counter;
				++offset;
				_edgeVertices.push_back(edgeVertex);
			}
		}
		veMap.length = counter;
		_vertexEdgesLookup.push_back(veMap);
		if (counter >= maxEdges) {
			maxEdges = counter;
			edgeMax = i;
		}
	}
}

void TetraTools::TriangleTopology::GenerateTriangleMap()
{
	std::cout<<"Generating TriangleMap..."<<std::endl;
	unsigned int numVerts = _vertices.size();
	unsigned int numTriangles = _triangles.size();
	unsigned int offset = 0;
	unsigned int maxTriangles = 0;
	unsigned int trianglesMax = 0;
	for (unsigned int i=0; i<numVerts; ++i) {
		PrimitivesPerVertex vtMap;
		unsigned int counter = 0;
		vtMap.offset = offset;
		for (unsigned int j=0; j<numTriangles; ++j) {
			const Triangle& t = _triangles[j];
			unsigned int vertInTri = 0;
			TriangleVertex triangleVertex;
			if (t.index[0] == i) {
				triangleVertex.triangleIndex = j;
				triangleVertex.indexInTriangle = vertInTri;
				++counter;
				++offset;
				_triangleVertices.push_back(triangleVertex);
			}
			++vertInTri;
			if (t.index[1] == i) {
				triangleVertex.triangleIndex = j;
				triangleVertex.indexInTriangle = vertInTri;
				++counter;
				++offset;
				_triangleVertices.push_back(triangleVertex);
			}
			++vertInTri;
			if (t.index[2] == i) {
				triangleVertex.triangleIndex = j;
				triangleVertex.indexInTriangle = vertInTri;
				++counter;
				++offset;
				_triangleVertices.push_back(triangleVertex);
			}
		}
		vtMap.length = counter;
		_vertexTrianglesLookup.push_back(vtMap);
		if (counter >= maxTriangles) {
			maxTriangles = counter;
			trianglesMax = i;
		}
	}
	std::cout<<"\tMax number of triangles connected to a vertex: "<<maxTriangles<<" at Vertex: "<<trianglesMax<<std::endl;
}

void TetraTools::TriangleTopology::GenerateBoundingBox()
{
	GenerateBoundingBox(_vertices);
}

void TetraTools::TriangleTopology::GenerateBoundingBox(const std::vector<Vec3f>& vertices_)
{
	std::vector<Vec3f>::const_iterator it;
	/// generate Bounding Box
	std::cout<<"Generating BoundingBox ..."<<std::endl;
	BoundingBox b;
	//b.min = Vec3f(FLT_MAX, FLT_MAX, FLT_MAX);
	b.min = Vec3f(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	//b.max = Vec3f(FLT_MIN, FLT_MIN, FLT_MIN);
	b.max = Vec3f(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
	float dist = 0;
	for (it=vertices_.begin(); it!=vertices_.end(); ++it)
	{
		Vec3f v = *it;
		float pd = v.squaredLength();
		if (pd > dist)
		{
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
	bc.center = Vec3f(b.min.x + dx/2, b.min.y + dy/2, b.min.z + dz / 2);
	const float lbcModifier = maxDist * 0.1f; /// add 10% volume for the larger bounding cube
	lbc.size = maxDist + lbcModifier;
	lbc.center = Vec3f(b.min.x + dx/2, b.min.y + dy/2, b.min.z + dz / 2);
	//std::cout<<"dx/dy/dz: "<<dx<<"/"<<dy<<"/"<<dz<<"; bc: "<<maxDist<<std::endl;
	std::cout<<"\t BBox: "<<bb.min<<"; "<<bb.max<<std::endl;
}

void TetraTools::TriangleTopology::Init(	const std::vector<Vec3f>& vertices_,
											const std::vector<Triangle>& triangles_,
											const bool fullUpdate_)
{
	Clear();
	//GenerateBoundingBox(vertices_);
	for (unsigned int i=0; i<vertices_.size(); ++i)
	{
		_vertices.push_back(vertices_[i]);
	}
	GenerateBoundingBox();
	std::vector<Triangle>::const_iterator tit;
	for (tit=triangles_.begin(); tit!=triangles_.end(); ++tit)
	{
		_triangles.push_back(*tit);
	}
	if (fullUpdate_)
	{
		GenerateEdges();
		//GenerateTriangleEdges();
		//GenerateEdgeMap();
		//GenerateTriangleMap();
		GenerateNormals();
	}
}

void TetraTools::TriangleTopology::Clear()
{
	_vertices.clear();
	_triangles.clear();
	_edges.clear();
	_triangleEdges.clear();
	_triangleVertices.clear();
	_edgeVertices.clear();
	_vertexEdgesLookup.clear();
	_vertexTrianglesLookup.clear();
}

void TetraTools::TriangleTopology::SetEdges(const std::vector<Edge>& edges_)
{
	_edges.clear();
	for (unsigned int i=0; i<edges_.size(); ++i)
	{
		_edges.push_back(edges_[i]);
	}
	_edgeVertices.clear();
	_vertexEdgesLookup.clear();
	_triangleEdges.clear();
}

const unsigned int TetraTools::TriangleTopology::FindEdgeByIndex(const unsigned int i0, const unsigned int i1)
{
	if (_edges.size() == 0)
	{
		GenerateEdges();
		if (_edges.size() == 0)
			return -1;
	}
	for (unsigned int i=0; i<_edges.size(); ++i)
	{
		const Edge e = _edges[i];
		if (((e.index[0] == i0) && (e.index[1] == i1)) || ((e.index[0] == i1) &&  (e.index[1] == i0))) {
			return i;
		}
	}
	return -1;
}
