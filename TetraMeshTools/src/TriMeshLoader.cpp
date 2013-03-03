/*
 * TriMeshLoader.cpp
 *
 *  Created on: Jan 04, 2013
 *      Author: Dennis Luebke
 */

#include <fstream>
#include "TetraMeshTools/TriMeshLoader.h"
#include "TriMesh.h"

TetraTools::TriMeshLoader::TriMeshLoader()
{

}

TetraTools::TriMeshLoader::~TriMeshLoader()
{
	_vertices.clear();
	_triangles.clear();
}

bool TetraTools::TriMeshLoader::loadFile(const std::string& path_and_filename)
{
	_vertices.clear();
	_triangles.clear();
	TriMesh* tm = TriMesh::read(path_and_filename.c_str());
	if (tm->vertices.size() == 0 || tm->faces.size() == 0)
	{
		std::cerr<<"Error loading surface mesh! Vertices or faces are empty..."<<std::endl;
		return false;
	}
	else
	{
		std::cout<<"Successfully loaded surface mesh with "<<tm->vertices.size()<<" vertices and "<<tm->faces.size()<<" faces."<<std::endl;
	}
	for (unsigned int i=0; i<tm->vertices.size(); ++i)
	{
		const point& p = tm->vertices[i];
		Vec3f v(p[0], p[1], p[2]);
		_vertices.push_back(v);
	}
	for (unsigned int i=0; i<tm->faces.size(); ++i)
	{
		const TriMesh::Face& f = tm->faces[i];
		Triangle t;
		t.index[0] = f[0];
		t.index[1] = f[1];
		t.index[2] = f[2];
		_triangles.push_back(t);
	}
	delete tm;
	return true;
}