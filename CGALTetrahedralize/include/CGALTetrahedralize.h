/*
 * CGALTetrahedralize.h
 *
 *	This is a wrapper interface for generating a tetrahedral mesh
 *	from a surface mesh using CGAL's 3D polyhedron triangulation.
 *	( http://www.cgal.org/ )
 *
 *  Created on: Dec. 06, 2012
 *      Author: Dennis Luebke
 */

#ifndef CGAL_TETRAHEDRALIZE_H
#define CGAL_TETRAHEDRALIZE_H

#include "TetraMeshTools/GeometryTypes.h"
#include <vector>
#include "TetraMeshTools/TetraToolsExports.h"

#ifndef BOOST_PARAMETER_MAX-ARITY
	#define BOOST_PARAMETER_MAX_ARITY 12
#endif

class DLL_EXPORT CGALTetrahedralize 
{
public:
	CGALTetrahedralize();
	~CGALTetrahedralize();

	/**
	 *	Main function: Generate a tetrahedral mesh from a triangle surface 
	 *	defined by a list of triangles and list of vertices.
	 *	Additionally we will set all necessary parameters for CGAL's meshing algorithm.
	 */
	void GenerateFromSurface(const std::vector<Triangle>& tris, const std::vector<Vec3f>& verts, const double cell_size_, const double facet_angle_, const double facet_size_, const double face_distance_, const double cell_radius_dege_ratio_);

	/**
	 *	Automatic conversion from the algorithm's ouput data to STL type vector
	 */
	std::vector<Vec3f>& GetTetraVertices();

	/**
	 *	Returns a list of the previously generated tetrahedra indices
	 */
	std::vector<Tetrahedron>& GetTetras();
	
	/*
	 *	Clear stored surface vertices and triangle indices.
	 */
	void clear();

private:
	std::vector<Vec3f> 			tetraPoints;
	std::vector<Tetrahedron> 	tetraIndices;
};

#endif // CGAL_TETRAHEDRALIZE_H
