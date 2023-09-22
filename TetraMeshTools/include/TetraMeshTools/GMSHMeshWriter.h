/**
 *	Class for writing GMSH files (.msh)
 *
 *  Created on: Aug 21, 2011
 *      Author: Dennis Luebke
 */

#ifndef GMSH_MESH_WRITER_H_
#define GMSH_MESH_WRITER_H_

#include "GeometryTypes.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "TetraToolsExports.h"

namespace TetraTools {
class DLL_EXPORT GMSHMeshWriter {
public:
  GMSHMeshWriter() {}

  bool SaveToFile(const std::string &fileName_, const std::vector<Vec3f> &verts,
                  const std::vector<Tetrahedron> &tetras);
};

} // namespace TetraTools

#endif // GMSH_MESH_WRITER_H_
