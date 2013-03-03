/*
 * TetrahedronTopology.h
 *
 *  Created on: Aug 19, 2011
 *      Author: Dennis Luebke
 *
 * This is a topology container for tetrahedral meshes based on the TriangleTopology.
 */

#include "TriangleTopology.h"

#include "TetraToolsExports.h"

namespace TetraTools
{
	/**
	 * This struct represents a Tetrahedron that is connected to a certain
	 * vertex (by index).
	 */
	struct TetrahedronVertex
	{
		unsigned int tetraIndex;
		unsigned int indexInTetra;
	};

	struct TetrahedronEdges
	{
		unsigned int index[6];
	};

	/**
	 * We want to store the Triangle indices for each tetrahedron
	 */
	struct TetrahedronTriangles
	{
		unsigned int index[4];
	};

	class DLL_EXPORT TetrahedronTopology : public TriangleTopology
	{
	protected:
		std::vector<Tetrahedron>			_tetrahedra;
		std::vector<TetrahedronVertex>		_tetraVertices;
		std::vector<PrimitivesPerVertex>	_vertexTetrahedraLookup;
		std::vector<TetrahedronEdges>		_tetraEdges;
		std::vector<TetrahedronTriangles>	_tetraTriangles;
		std::vector<Triangle>				_surfaceTriangles;

		/**
		 * Generate Triangles from the tetrahedra.
		 */
		void GenerateTriangles();

		/**
		 * Generate Edges from Tetrahedra.
		 * This will usually be faster than generating the Edges from the Triangles.
         */
		//virtual void GenerateEdges();

		void GenerateTetraEdges();

		void GenerateTetrahedronMap();

        /**
         * Surface triangles will be generated based on the fact that surface
         * triangles do not belong to more than one tetrahedra.
         */
		void GenerateSurfaceTriangles();

		void GenerateTetraTriangles();

		/**
		 * Swap edge order to have the smaller index in the first position
		 */
		void OrderEdge(Edge& e_)
		{
			unsigned int swap;
			if (e_.index[0] > e_.index[1])
			{
				swap = e_.index[0];
				e_.index[0] = e_.index[1];
				e_.index[1] = swap;
			}
		}

	public:

		/// Constructors
		TetrahedronTopology();

		TetrahedronTopology(const std::vector<Vec3f>& vertices_, const std::vector<Tetrahedron>& tetras_, const bool complete_);

		~TetrahedronTopology();

		virtual void Init(const std::vector<Vec3f>& vertices_, const std::vector<Tetrahedron>& tetras_, const bool complete_ = false);

		virtual void Clear();

		const std::vector<Tetrahedron>& GetTetrahedra()
		{
			return _tetrahedra;
		}

		const std::vector<TetrahedronEdges>& GetTetraEdges()
		{
			if (_tetraEdges.size() == 0)
				GenerateTetraEdges();
			return _tetraEdges;
		}

		const std::vector<TetrahedronVertex>& GetTetrasPerVertices()
		{
			if (_tetraVertices.size() == 0)
				GenerateTetrahedronMap();
			return _tetraVertices;
		}

		const std::vector<PrimitivesPerVertex>& GetTetrasPerVertexLookupTable()
		{
			if (_vertexTetrahedraLookup.size() == 0)
				GenerateTetrahedronMap();
			return _vertexTetrahedraLookup;
		}

		const std::vector<Triangle>& GetSurfaceTriangles()
		{
			if (_surfaceTriangles.size() == 0)
				GenerateSurfaceTriangles();
			return _surfaceTriangles;
		}

		unsigned int GetNumSurfaceTriangles()
		{
			return _surfaceTriangles.size();
		}

		const std::vector<TetrahedronTriangles>& GetTetraTriangles()
		{
			if (_tetraTriangles.size() == 0)
				GenerateTetraTriangles();
			return _tetraTriangles;
		}

		unsigned int GetNumTetras()
		{
			return _tetrahedra.size();
		}
	};

}	/// end namespace TetraTools
