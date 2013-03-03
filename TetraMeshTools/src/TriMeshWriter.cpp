/*
 * TriMeshWriter.cpp
 *
 *  Created on: Feb 09, 2013
 *      Author: Dennis Luebke
 */

#include <fstream>
#include "TetraMeshTools/TriMeshWriter.h"
#include "TriMesh.h"

TetraTools::TriMeshWriter::TriMeshWriter()
{

}

TetraTools::TriMeshWriter::~TriMeshWriter()
{
	
}

bool TetraTools::TriMeshWriter::writeFile(const std::string& path_and_filename, const std::vector<Vec3f>& verts_, const std::vector<Triangle>& triangles_)
{
	TriMesh* tm = new TriMesh();
	tm->vertices.clear();
	for (unsigned int i=0; i<verts_.size(); ++i)
	{
		point p;
		p[0] = verts_[i].x;
		p[1] = verts_[i].y;
		p[2] = verts_[i].z;
		tm->vertices.push_back(p);
	}
	for (unsigned int i=0; i<triangles_.size(); ++i)
	{
		TriMesh::Face f;
		f[0] = triangles_[i].index[0];
		f[1] = triangles_[i].index[1];
		f[2] = triangles_[i].index[2];
		tm->faces.push_back(f);
	}
	bool success = tm->write(path_and_filename.c_str());
	delete tm;
	return success;
}
