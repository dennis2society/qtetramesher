/**
 *	Class for writing Tetgen files (.node + .ele)
 *
 *  Created on: May 19, 2013
 *      Author: Dennis Luebke
 */

#ifndef TETGEN_WRITER_H_
#define TETGEN_WRITER_H_

#include <string>
#include <vector>
#include "GeometryTypes.h"

#include "TetraToolsExports.h"

namespace TetraTools
{
class DLL_EXPORT TetgenWriter
{
public:
	TetgenWriter()
	{}

	/**
	 *	As we have to write two separate files with different extension for Tetgen compliance, the 
	 *	filename argument will have no extension. Required extensions will be added automatically.
	 */
	bool SaveToFile(const std::string& fileName_, const std::vector<Vec3f>& verts, const std::vector<Tetrahedron>& tetras);
};

} /// end namespace TetraTools

#endif // TETGEN_WRITER_H_