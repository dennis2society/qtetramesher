/*
 * TriMeshWriter.h
 *
 *  Created on: Feb 09, 2013
 *      Author: Dennis Luebke
 *
 * Base class for mesh saving to files using the trimesh2 library.
 * http://gfx.cs.princeton.edu/proj/trimesh2/
 *
 */

#ifndef TRIMESHWRITER_H_
#define TRIMESHWRITER_H_

#include "TetraMeshTools/GeometryTypes.h"
#include "TetraMeshTools/TetraToolsExports.h"
#include <vector>

namespace TetraTools
{

class DLL_EXPORT TriMeshWriter
{
public:
	TriMeshWriter();

	~TriMeshWriter();

	// loads the file on object construction
	bool writeFile(const std::string& path_and_filename, const std::vector<Vec3f>& verts_, const std::vector<Triangle>& triangles_);

};

}	/// end namespace TetraTools

#endif /* TRIMESHWRITER_H_ */
