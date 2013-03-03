/*
 * TetrahedronTopology.cpp
 *
 *  Created on: Aug 20, 2011
 *      Author: Dennis Luebke
 */

#include "TetrahedronTopology.h"
#include <map>
#include <algorithm>
#ifndef WIN32
#include <cfloat>
#include <math.h>
#else
#include <float.h>
#endif

TetraTools::TetrahedronTopology::TetrahedronTopology()
{
	Clear();
}

TetraTools::TetrahedronTopology::TetrahedronTopology(	const std::vector<Vec3f>& vertices_,
														const std::vector<Tetrahedron>& tetras_,
														const bool complete_)
{
	Clear();
	Init(vertices_, tetras_, complete_);
}

TetraTools::TetrahedronTopology::~TetrahedronTopology()
{
	Clear();
}

void TetraTools::TetrahedronTopology::Init(	const std::vector<Vec3f>& vertices_,
											const std::vector<Tetrahedron>& tetras_,
											const bool complete_)
{
	/// generate Bounding Box
	Clear();
	//GenerateBoundingBox(vertices_);
	for (unsigned int i=0; i<vertices_.size(); ++i)
	{
		_vertices.push_back(vertices_[i]);
	}
	GenerateBoundingBox();
	std::cout<<"Num Tetras: "<<tetras_.size()<<std::endl;
	for (unsigned int i=0; i<tetras_.size(); ++i)
	{
		_tetrahedra.push_back(tetras_[i]);
	}
	GenerateTriangles();
	GenerateEdges();
	if (complete_)
	{
		//GenerateEdgeMap();
		//GenerateTetraEdges();
		//GenerateTetraTriangles();
		//GenerateTetrahedronMap();
		//GenerateTriangleEdges();
		//GenerateTriangleMap();
		//GenerateSurfaceTriangles();
	}
}

void TetraTools::TetrahedronTopology::Clear()
{
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

void TetraTools::TetrahedronTopology::GenerateTriangles()
{
	// create a temporary map to find redundant triangles
	std::cout<<"Generating Triangles from Tetrahedra..."<<std::endl;
	std::map<Triangle,unsigned int> triangleMap;
	std::map<Triangle,unsigned int>::iterator itt;
	std::map<Triangle,unsigned int>::iterator itr;
	Triangle tr, rtr;
	int triangleIndex;
	unsigned int v[3],val;
	for (unsigned int i=0; i<_tetrahedra.size(); ++i)
	{
		const Tetrahedron& t = _tetrahedra[i];
		for (unsigned int j=0;j<4;++j)
		{
			if (j%2)
			{
				v[0] = t.index[(j+1)%4];
				v[1] = t.index[(j+2)%4];
				v[2] = t.index[(j+3)%4];
			}
			else
			{
				v[0] = t.index[(j+1)%4];
				v[2] = t.index[(j+2)%4];
				v[1] = t.index[(j+3)%4];
			}
			//		std::sort(v,v+2);
			// sort v such that v[0] is the smallest one
			while ((v[0]>v[1]) || (v[0]>v[2]))
			{
				val = v[0];
				v[0] = v[1];
				v[1] = v[2];
				v[2]=val;
			}
			// check if a triangle with an opposite orientation already exists
			tr.index[0] = v[0];
			tr.index[1] = v[2];
			tr.index[2] = v[1];
			rtr.index[0] = v[0];
			rtr.index[1] = v[1];
			rtr.index[2] = v[2];
			itt = triangleMap.find(tr);
			itr = triangleMap.find(rtr);
			if (itt == triangleMap.end() && itr == triangleMap.end())
			{
				// edge not in edgeMap so create a new one
				triangleIndex = _triangles.size();
				triangleMap[tr] = triangleIndex;
				_triangles.push_back(tr);
			}
			else
			{
				triangleIndex = -1; //itt->second;
			}
		}
	}
	std::cout<<"\tNum Triangles in TetraMesh: "<<_triangles.size()<<std::endl;
	std::sort(_triangles.begin(), _triangles.end());
}

void TetraTools::TetrahedronTopology::GenerateTetraEdges()
{
	std::cout<<"Generating TetraEdges..."<<std::endl;
	if (_tetraEdges.size() != 0)
		_tetraEdges.clear();
	/// generate Edge map for faster lookup
	std::map<Edge, unsigned int> edgeMap;
	std::map<Edge, unsigned int>::iterator itt;
	for (unsigned int i=0; i<_edges.size(); ++i)
	{
		const Edge& e = _edges[i];
		edgeMap[e] = i;
	}
	Edge tmpEdge;
	for (unsigned int i=0; i<_tetrahedra.size(); ++i)
	{
		const Tetrahedron& t = _tetrahedra[i];
		TetrahedronEdges te;

		/// find the six edges in the edge map
		/// swap edge indices so that the smaller index is in the first position
		tmpEdge.index[0] = t.index[0];
		tmpEdge.index[1] = t.index[1];
		OrderEdge(tmpEdge);
		itt = edgeMap.find(tmpEdge);
		te.index[0] = itt->second;

		tmpEdge.index[0] = t.index[0];
		tmpEdge.index[1] = t.index[2];
		OrderEdge(tmpEdge);
		itt = edgeMap.find(tmpEdge);
		te.index[1] = itt->second;

		tmpEdge.index[0] = t.index[0];
		tmpEdge.index[1] = t.index[3];
		OrderEdge(tmpEdge);
		itt = edgeMap.find(tmpEdge);
		te.index[2] = itt->second;

		tmpEdge.index[0] = t.index[1];
		tmpEdge.index[1] = t.index[2];
		OrderEdge(tmpEdge);
		itt = edgeMap.find(tmpEdge);
		te.index[3] = itt->second;

		tmpEdge.index[0] = t.index[1];
		tmpEdge.index[1] = t.index[3];
		OrderEdge(tmpEdge);
		itt = edgeMap.find(tmpEdge);
		te.index[4] = itt->second;

		tmpEdge.index[0] = t.index[2];
		tmpEdge.index[1] = t.index[3];
		OrderEdge(tmpEdge);
		itt = edgeMap.find(tmpEdge);
		te.index[5] = itt->second;

		_tetraEdges.push_back(te);
	}
}

void TetraTools::TetrahedronTopology::GenerateTetrahedronMap()
{
	std::cout<<"Generating TetraMap..."<<std::endl;
	unsigned int numVerts = _vertices.size();
	unsigned int numTetras = _tetrahedra.size();
	unsigned int offset = 0;
	unsigned int maxTetras = 0;
	unsigned int tetrasMax = 0;
	for (unsigned int i=0; i<numVerts; ++i) {
		PrimitivesPerVertex vtMap;
		unsigned int counter = 0;
		vtMap.offset = offset;
		for (unsigned int j=0; j<numTetras; ++j) {
			const Tetrahedron& t = _tetrahedra[j];
			unsigned int vertInTetra = 0;
			TetrahedronVertex tetraVertex;
			if (t.index[0] == i) {
				tetraVertex.tetraIndex = j;
				tetraVertex.indexInTetra = vertInTetra;
				++counter;
				++offset;
				_tetraVertices.push_back(tetraVertex);
			}
			++vertInTetra;
			if (t.index[1] == i) {
				tetraVertex.tetraIndex = j;
				tetraVertex.indexInTetra = vertInTetra;
				++counter;
				++offset;
				_tetraVertices.push_back(tetraVertex);
			}
			++vertInTetra;
			if (t.index[2] == i) {
				tetraVertex.tetraIndex = j;
				tetraVertex.indexInTetra = vertInTetra;
				++counter;
				++offset;
				_tetraVertices.push_back(tetraVertex);
			}
			++vertInTetra;
			if (t.index[3] == i) {
				tetraVertex.tetraIndex = j;
				tetraVertex.indexInTetra = vertInTetra;
				++counter;
				++offset;
				_tetraVertices.push_back(tetraVertex);
			}
		}
		vtMap.length = counter;
		_vertexTetrahedraLookup.push_back(vtMap);
		if (counter >= maxTetras) {
			maxTetras = counter;
			tetrasMax = i;
		}
	}
	std::cout<<"\tMax number of tetrahedra connected to a vertex: "<<maxTetras<<" at Vertex: "<<tetrasMax<<std::endl;
}

void TetraTools::TetrahedronTopology::GenerateSurfaceTriangles()
{
	std::cout<<"Generating SurfaceTriangles from tetrahedra..."<<std::endl;
	/// generate Triangle-To-Indices map for faster lookup
	std::map<Triangle, unsigned int> triangleMap;
	std::map<Triangle, unsigned int>::iterator itm;
	std::map<Triangle, unsigned int> tmpTriangles;
	std::map<Triangle, unsigned int>::iterator itt;
	std::map<Triangle, unsigned int> trianglesBlacklist;
	std::map<Triangle, unsigned int>::iterator itb;
	for (unsigned int i=0; i<_triangles.size(); ++i)
	{
		const Triangle& e = _triangles[i];
		triangleMap[e] = i;
	}
	Triangle tri, rTri;
	unsigned int v[3], val;
	for (unsigned int k=0; k<_tetrahedra.size(); ++k) {
		const Tetrahedron& t = _tetrahedra[k];
		for (unsigned int f=0; f<4; ++f) {
			if (f%2)
			{
				v[0] = t.index[(f+1)%4];
				v[1] = t.index[(f+2)%4];
				v[2] = t.index[(f+3)%4];
			}
			else
			{
				v[0] = t.index[(f+1)%4];
				v[2] = t.index[(f+2)%4];
				v[1] = t.index[(f+3)%4];
			}
			//		std::sort(v,v+2);
			// sort v such that v[0] is the smallest one
			while ((v[0]>v[1]) || (v[0]>v[2]))
			{
				val = v[0];
				v[0] = v[1];
				v[1] = v[2];
				v[2]=val;
			}
			rTri.index[0] = v[0];
			rTri.index[1] = v[1];
			rTri.index[2] = v[2];
			tri.index[0] = v[0];
			tri.index[1] = v[2];
			tri.index[2] = v[1];
			/// assume that itm always exists due to previous triangle generation
			itm = triangleMap.find(tri);
			itb = trianglesBlacklist.find(tri);
			/// check if triangle is already in blacklist
			if (itb == trianglesBlacklist.end())
			{
				const unsigned int triangleIndex = itm->second;
				trianglesBlacklist[tri] = triangleIndex;
				trianglesBlacklist[rTri] = triangleIndex;
				itt = tmpTriangles.find(tri);
				/// check if triangle is in tmpList, remove if necessary
				if (itt == tmpTriangles.end())
				{
					tmpTriangles[tri] = triangleIndex;
				}
				else
				{
					tmpTriangles.erase(itt);
				}
			}
			else
			{
				itt = tmpTriangles.find(tri);
				/// check if triangle is in tmpList, remove if necessary
				if (itt == tmpTriangles.end())
				{
				}
				else
				{
					tmpTriangles.erase(itt);
				}
				itt = tmpTriangles.find(rTri);
				/// check if triangle is in tmpList, remove if necessary
				if (itt == tmpTriangles.end())
				{
				}
				else
				{
					tmpTriangles.erase(itt);
				}
			}
		}
	}
	std::cout<<"\tNum surface triangles: "<<tmpTriangles.size()<<std::endl;
	for (itt=tmpTriangles.begin(); itt!=tmpTriangles.end(); ++itt)
	{
		const Triangle& t = itt->first;
		_surfaceTriangles.push_back(t);
	}
}

void TetraTools::TetrahedronTopology::GenerateTetraTriangles()
{
	std::cout<<"Generating triangles per tetrahedron..."<<std::endl;
	if (_tetraTriangles.size() != 0)
		_tetraTriangles.clear();
	/// generate Triangle-To-Indices map for faster lookup
	std::map<Triangle, unsigned int> triangleMap;
	std::map<Triangle, unsigned int>::iterator itm;
	for (unsigned int i=0; i<_triangles.size(); ++i)
	{
		const Triangle& e = _triangles[i];
		triangleMap[e] = i;
	}
	Triangle tri, rTri;
	unsigned int v[3], val;
	for (unsigned int k=0; k<_tetrahedra.size(); ++k) {
		const Tetrahedron& t = _tetrahedra[k];
		TetrahedronTriangles tt;
		for (unsigned int f=0; f<4; ++f) {
			if (f%2)
			{
				v[0] = t.index[(f+1)%4];
				v[1] = t.index[(f+2)%4];
				v[2] = t.index[(f+3)%4];
			}
			else
			{
				v[0] = t.index[(f+1)%4];
				v[2] = t.index[(f+2)%4];
				v[1] = t.index[(f+3)%4];
			}
			//		std::sort(v,v+2);
			// sort v such that v[0] is the smallest one
			while ((v[0]>v[1]) || (v[0]>v[2]))
			{
				val = v[0];
				v[0] = v[1];
				v[1] = v[2];
				v[2]=val;
			}
			tri.index[0] = v[0];
			tri.index[1] = v[2];
			tri.index[2] = v[1];
			rTri.index[0] = v[0];
			rTri.index[1] = v[1];
			rTri.index[2] = v[2];
			itm = triangleMap.find(tri);
			/// assume that itm always exists. If first find fails, the triangle
			/// should exist in the reverse orientation
			if (itm == triangleMap.end())
				itm = triangleMap.find(rTri);
			tt.index[f] = itm->second;
		}
		_tetraTriangles.push_back(tt);
	}
}

