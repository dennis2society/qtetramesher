/*
 * TriMeshLoader.h
 *
 *  Created on: Jan 04, 2013
 *      Author: Dennis Luebke
 *
 * Base class for mesh loading from files using the trimesh2 library.
 * http://gfx.cs.princeton.edu/proj/trimesh2/
 *
 */

#ifndef TRIMESHLOADER_H_
#define TRIMESHLOADER_H_

#include "TetraMeshTools/GeometryTypes.h"
#include "TetraMeshTools/TetraToolsExports.h"
#include <vector>

namespace TetraTools
{

class DLL_EXPORT TriMeshLoader
{
public:
	TriMeshLoader();

	~TriMeshLoader();

	const std::vector<Vec3f>& 	GetVertices()
	{
		return _vertices;
	}

	const std::vector<Triangle>	GetTriangles()
	{
		return _triangles;
	}

	// loads the file on object construction
	bool loadFile(const std::string& path_and_filename);

protected:
	std::vector<Vec3f>	_vertices;
	std::vector<Triangle>	_triangles;

};

}	/// end namespace TetraTools

#endif /* TRIMESHLOADER_H_ */
