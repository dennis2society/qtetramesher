/**
 *	Class for writing GMSH files (.msh)
 *
 *  Created on: Aug 21, 2011
 *      Author: Dennis Luebke
 */

#ifndef GMSH_MESH_WRITER_H_
#define GMSH_MESH_WRITER_H_

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "GeometryTypes.h"

#include "TetraToolsExports.h"

namespace TetraTools
{
class DLL_EXPORT GMSHMeshWriter
{
public:
	GMSHMeshWriter()
	{}

	bool SaveToFile(const std::string& fileName_, const std::vector<Vec3f>& verts, const std::vector<Tetrahedron>& tetras);
};

} /// end namespace TetraTools

#endif // GMSH_MESH_WRITER_H_