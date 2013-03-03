/*
 * SofaTetraStuffing.h
 *
 *	This is a wrapper interface for generating a tetrahedral mesh
 *	from a surface mesh using Jonathan Shewchuk's Isosurface TetraStuffing
 *	algorithm ( http://www.cs.berkeley.edu/~jrs/papers/stuffing.pdf ).
 *	Implementation taken from the SOFA framework ( http://www.sofa-framework.org/ ).
 *
 *  Created on: Nov 26, 2011
 *      Author: Dennis Luebke
 */

#ifndef SOFA_TETRA_STUFFING_H
#define SOFA_TETRA_STUFFING_H

#include "TetraMeshTools/GeometryTypes.h"
#include <vector>
#include "TetraMeshTools/TetraToolsExports.h"

class DLL_EXPORT SofaTetraStuffing 
{
public:
	SofaTetraStuffing()
	{}

	/**
	 *	Main function: Generate a tetrahedral mesh from a triangle surface 
	 *	defined by a list of triangles and list of vertices.
	 *	Additionally we will set all necessary parameters for Shewchuk's algorithm.
	 */
	void GenerateFromSurface(const std::vector<Triangle>& tris, const std::vector<Vec3f>& verts, const float size_, const float as_, const float al_, const bool snapToSurface, const bool splitTetras);

	/**
	 *	Automatic conversion from the algorithm's ouput data to STL type vector
	 */
	std::vector<Vec3f>& GetTetraVertices();

	/**
	 *	Returns a list of the previously generated tetrahedra indices
	 */
	std::vector<Tetrahedron>& GetTetras();

private:
	std::vector<Vec3f> tetraPoints;
	std::vector<Tetrahedron> tetraIndices;

};

#endif // SOFA_TETRA_STUFFING_H
