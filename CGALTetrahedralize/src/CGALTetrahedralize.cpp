/*
 *  Created on: Dec. 06, 2012
 *      Author: Dennis Luebke
 */

#include "CGALTetrahedralize.h"
#ifdef _WIN32
#include <windows.h>
#endif

// update for Ubuntu 16.04 or CGAL version >= 4.8*
#ifndef BOOST_PARAMETER_MAX_ARITY
		#define BOOST_PARAMETER_MAX_ARITY 12
#endif

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#include <CGAL/Simple_cartesian.h>

#include <CGAL/Polyhedron_incremental_builder_3.h>
#include <CGAL/Polyhedron_3.h>

#include <CGAL/Mesh_triangulation_3.h>
#include <CGAL/Mesh_complex_3_in_triangulation_3.h>
#include <CGAL/Mesh_criteria_3.h>
#include <CGAL/Triangulation_cell_base_3.h>

#include <CGAL/Polyhedral_mesh_domain_3.h>
#include <CGAL/make_mesh_3.h>
#include <CGAL/refine_mesh_3.h>

// IO
#include <CGAL/IO/Polyhedron_iostream.h>
#include <iostream>
#include <map>

// Domain
typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel2;
typedef CGAL::Polyhedron_3<Kernel2> Polyhedron2;
typedef CGAL::Simple_cartesian<double>     Kernel;
typedef CGAL::Polyhedron_3<Kernel>         Polyhedron;
typedef CGAL::Polyhedral_mesh_domain_3<Polyhedron2, Kernel2> Mesh_domain;
typedef Polyhedron::Vertex_iterator        Vertex_iterator;
typedef Polyhedron::Facet_iterator         Triangle_iterator;
typedef Polyhedron::Halfedge_around_facet_circulator Halfedge_facet_circulator;
typedef Polyhedron::HalfedgeDS             HalfedgeDS;

// Triangulation
typedef CGAL::Mesh_triangulation_3<Mesh_domain>::type Tr;
typedef CGAL::Mesh_complex_3_in_triangulation_3<Tr> C3t3;
typedef CGAL::Triangulation_cell_base_3<Tr>	Cell_Base;

typedef CGAL::Mesh_complex_3_in_triangulation_3<Tr>::Vertices_in_complex_iterator Complex_Vertex_Iterator;
typedef CGAL::Mesh_complex_3_in_triangulation_3<Tr>::Cells_in_complex_iterator		Complex_Cell_Iterator;

typedef C3t3::Cell_iterator Cell_iterator;

typedef Tr::Finite_vertices_iterator Finite_vertices_iterator;
typedef Tr::Vertex_handle Vertex_handle;
typedef Tr::Point Point_3;

// Criteria
typedef CGAL::Mesh_criteria_3<Tr> Mesh_criteria;

// To avoid verbose function and named parameters call
using namespace CGAL::parameters;


/*
 * 	The following section was found in the CGAL mailing list:
 *	http://cgal-discuss.949826.n4.nabble.com/Example-Convert-polyhedron-from-one-kernel-to-another-td4514497.html
 */
// Can be used to convert polyhedron from exact to inexact and vice-versa
template <class Polyhedron_input, class Polyhedron_output>
struct Copy_polyhedron_to : public CGAL::Modifier_base<typename Polyhedron_output::HalfedgeDS>
{
	Copy_polyhedron_to(const Polyhedron_input& in_poly) : in_poly(in_poly) {}

	void operator()(typename Polyhedron_output::HalfedgeDS& out_hds)
	{
		typedef typename Polyhedron_output::HalfedgeDS Output_HDS;

		CGAL::Polyhedron_incremental_builder_3<Output_HDS> builder(out_hds);

		typedef typename Polyhedron_input::Vertex_const_iterator Vertex_const_iterator;
		typedef typename Polyhedron_input::Facet_const_iterator  Facet_const_iterator;
		typedef typename Polyhedron_input::Halfedge_around_facet_const_circulator HFCC;

		builder.begin_surface(in_poly.size_of_vertices(), in_poly.size_of_facets(), in_poly.size_of_halfedges());

		for(Vertex_const_iterator
				vi = in_poly.vertices_begin(), end = in_poly.vertices_end();
				vi != end ; ++vi)
		{
				typename Polyhedron_output::Point_3 p(::CGAL::to_double( vi->point().x()),
						::CGAL::to_double( vi->point().y()),
						::CGAL::to_double( vi->point().z()));
				builder.add_vertex(p);
		}

		typedef CGAL::Inverse_index<Vertex_const_iterator> Index;
		Index index( in_poly.vertices_begin(), in_poly.vertices_end());

		for(Facet_const_iterator
				fi = in_poly.facets_begin(), end = in_poly.facets_end();
				fi != end; ++fi)
		{
				HFCC hc = fi->facet_begin();
				HFCC hc_end = hc;
				builder.begin_facet ();
				do {
						builder.add_vertex_to_facet(index[hc->vertex()]);
						++hc;
				} while( hc != hc_end);
				builder.end_facet();
		}
		builder.end_surface();
	} // end operator()(..)
private:
	const Polyhedron_input& in_poly;
}; // end Copy_polyhedron_to<>

template <class Poly_B, class Poly_A>
void poly_copy(Poly_B& poly_b, const Poly_A& poly_a)
{
        poly_b.clear();
        Copy_polyhedron_to<Poly_A, Poly_B> modifier(poly_a);
        poly_b.delegate(modifier);
} 

/*
 *	This is used to create the CGAL compliant surface mesh from the list of vertices and
 *	triangle indices:
 *
 * 	The following section was found in the CGAL documentation for CGAL::Polyhedron_incremental_builder_3.
 */
// A modifier creating a triangle with the incremental builder.
template <class HDS>
class Build_triangle : public CGAL::Modifier_base<HDS> 
{

private:
	std::vector<Triangle>		tris;
	std::vector<Vec3f>			verts;
	
public:
	Build_triangle(const std::vector<Triangle>& tris_, const std::vector<Vec3f>& verts_) 
	{
		tris = tris_;
		verts = verts_;
	}
	void operator()( HDS& hds) 
	{
		if (tris.size() == 0 || verts.size() == 0)
		{
			std::cerr<<"ERROR in CGALTetrahedralize! Vertices or triangles are empty..."<<std::endl;
			return;
		}
	
		// Postcondition: `hds' is a valid polyhedral surface.
		CGAL::Polyhedron_incremental_builder_3<HDS> B( hds, true);
		//B.begin_surface( verts.size(), tris.size(), numHalfEdges);
		B.begin_surface( verts.size(), tris.size());
		typedef typename HDS::Vertex   Vertex;
		typedef typename Vertex::Point Point;
		// add vertices to CGAL data structure
		for (unsigned int i=0; i<verts.size(); ++i)
		{
			const Vec3f& v = verts[i];
			B.add_vertex( Point(v.x, v.y, v.z));
		}

		// add triangles to CGAL data structure
		for (unsigned int i=0; i<tris.size(); ++i)
		{
			const Triangle& t = tris[i];
			B.begin_facet();
			B.add_vertex_to_facet(t.index[0]);
			B.add_vertex_to_facet(t.index[1]);
			B.add_vertex_to_facet(t.index[2]);
			B.end_facet();
			if (B.error())
			{
				std::cout<<"Error in Facet_Builder..."<<std::endl;
				return;
			}
		}
		B.end_surface();
	}
};

CGALTetrahedralize::CGALTetrahedralize()
{
	//clear();
}

void CGALTetrahedralize::clear()
{
	tetraPoints.clear();
	tetraIndices.clear();
}

CGALTetrahedralize::~CGALTetrahedralize()
{
	clear();
}


/// Helpful documentation note for self:
/// http://doc.cgal.org/latest/Mesh_3/index.html#Chapter_3D_Mesh_Generation

void CGALTetrahedralize::GenerateFromSurface(const std::vector<Triangle>& tris, const std::vector<Vec3f>& verts, const double cell_size_, const double facet_angle_, const double facet_size_, const double face_distance_, const double cell_radius_edge_ratio_)
{
	std::cout<<"Generating CGAL surface mesh from our own data structure..."<<std::endl;
	Polyhedron2 polyhedron;
	Polyhedron  tmpPoly;
	// generate the CGAL compliant mesh from triangle indices and vertices
	Build_triangle<HalfedgeDS> triangle(tris, verts);
	// transform the generated mesh from exact to inexact mesh criteria (this is required to generate the tetrahedral mesh)
	tmpPoly.delegate( triangle);
    //CGAL_assertion( polyhedron.is_triangle( polyhedron.halfedges_begin()));
    
	//std::cout<<"Successfully generated CGAL mesh from custom surface..."<<std::endl;

	//std::cout<<"Copying CGAL surface to new structure to generate tetrahedral mesh mesh from surface..."<<std::endl;
    poly_copy(polyhedron, tmpPoly);
    
	unsigned int i=0;
	
	// Create domain
	std::cout<<"Creating domain..."<<std::endl;
	Mesh_domain domain(polyhedron);

	// Mesh criteria (no cell_size set)
	std::cout<<"Mesh criteria..."<<std::endl;
	Mesh_criteria criteria(facet_angle=facet_angle_, facet_size=facet_size_, facet_distance=face_distance_, cell_radius_edge_ratio=cell_radius_edge_ratio_, cell_size=cell_size_);
	//Mesh_criteria criteria(cell_size=0.1, cell_radius_edge_ratio=3);

	// Mesh generation
	std::cout<<"Making mesh (this might take a while depending on the size of the surface mesh...)"<<std::endl;
	C3t3 c3t3 = CGAL::make_mesh_3<C3t3>(domain, criteria, no_perturb(), no_exude());

	std::cout<<"C3T3 Number of cells : "<<c3t3.number_of_cells()<<std::endl;
/*	
	Mesh_criteria new_criteria(cell_radius_edge_ratio=3, cell_size=0.03);
	// Mesh refinement
	std::cout<<"Refining mesh #2..."<<std::endl;
	CGAL::refine_mesh_3(c3t3, domain, new_criteria);
	
	std::cout<<"C3T3 Number of cells after refining: "<<c3t3.number_of_cells()<<std::endl;
*/
	// clear all existing output data structures
	clear();
	
	// Copy all data from CGAL data structures to our own structure.
	Tr t = c3t3.triangulation();
	i = 0;
	//std::cout<<"NumVerts: "<<t.number_of_vertices()<<std::endl;
	// Vertex map for storing the vertex indices (these are needed to generate the triangle indices from the vertex values)
	std::map<Point_3, int> V;
	//for (Tr::All_vertices_iterator it=t.all_vertices_begin(); it != t.all_vertices_end(); ++it)
	for( Finite_vertices_iterator it = t.finite_vertices_begin(); it != t.finite_vertices_end(); ++it)
	{
		// add the current point to the vertex map to re-use this map to generate the triangle-indices afterwards.
		V[it->point()] = i;
		Vec3f v;
		v.x = it->point().x();
		v.y = it->point().y();
		v.z = it->point().z();
		tetraPoints.push_back(v);
		++i;
	}
	i = 0;
	for (Complex_Cell_Iterator it = c3t3.cells_in_complex_begin(); it != c3t3.cells_in_complex_end(); ++it)
	//for (Tr::All_cells_iterator it = t.all_cells_begin(); it != t.all_cells_end(); ++it)
	{
		//const Tr::Cell c(*it);
		/*
		const C3t3::Vertex_handle v0 = c.vertex(0);
		const C3t3::Vertex_handle v1 = c.vertex(1);
		const C3t3::Vertex_handle v2 = c.vertex(2);
		const C3t3::Vertex_handle v3 = c.vertex(3);
		*/
		Tetrahedron tet;
		for (int j=0; j<4; ++j)
		{
			tet.index[j] = V[it->vertex(j)->point()];
		}
		tetraIndices.push_back(tet);
		++i;
	}
}


std::vector<Vec3f>& CGALTetrahedralize::GetTetraVertices()
{
	return tetraPoints;
}


std::vector<Tetrahedron>& CGALTetrahedralize::GetTetras()
{
	return tetraIndices;
}
