/**
 * TriangleTopology.h
 *
 *
 *  Created on: Aug 19, 2011
 *      Author: Dennis Luebke
 *
 * This is a topology container for triangle meshes.
 * This class includes data-structures for vertices, edge- and
 * triangle-indices and is useful for working with surface-meshes.
 * This class is also the foundation for the tetrahedron topology container.
 *
 * Additionally there are some convenience functions for retrieving
 * advanced data-structures like edges-in-triangles or
 * triangles-per-vertex lists. This might be useful for parallelized
 * algorithms or arbitrary look-up.
 *
 * This implementation fully relies on STL libraries and functions
 * and simple Vec3f types and can be easily adapted to different applications.
 *
 * Additionally this container will automatically detect topology changes
 * and re-generate any relevant data-structures when necessary.
 */

#include <vector>
#include "GeometryTypes.h"

#include "TetraToolsExports.h"

namespace TetraTools
{

	/**
	 * This struct contains an Edge index whose corresponding Edge
	 * is connected to a certain vertex.
	 */
	struct EdgeVertex
	{
		unsigned int edgeIndex;		///	The index of the edge
		unsigned int indexInEdge;	/// This can be either 0 or 1 and describes
									/// if the vertex is the first
	};

	/**
	 * This struct represents a Triangle that is connected to a certain
	 * vertex (by index). This might be useful for calculating the interpolated
	 * normal from the triangles that are connected to a certain vertex.
	 */
	struct TriangleVertex
	{
		unsigned int triangleIndex;
		unsigned int indexInTriangle;
	};

	/**
	 * This struct contains an offset and a length to describe
	 * how many edges or triangles are connected to a vertex.
	 * The offset will indicate the starting index in
	 * EdgeVertex or TriangleVertex list.
	 */
	struct PrimitivesPerVertex
	{
		unsigned int offset;		/// offset in the PrimitiveVertex list
		unsigned int length;		/// length of the segment in the list
	};

	/**
	 * Contains references to the three Edge-indices in a Triangle
	 */
	struct TriangleEdges
	{
		unsigned int index[3];

		friend std::ostream& operator<<( std::ostream& out, const TriangleEdges& te)
		{
			out<<te.index[0]<<"/"<<te.index[1]<<"/"<<te.index[2];
			return out;
		}
	};

    class DLL_EXPORT TriangleTopology
	{
	protected:
		/// members
		std::vector<Vec3f>					_vertices;				/// the list of vertices
		std::vector<Vec3f>					_normals;				/// the list of normals
		std::vector<Edge>					_edges;					/// the list of edges
		std::vector<Triangle>				_triangles;				/// the list of triangles
		std::vector<EdgeVertex>				_edgeVertices;			/// edges-per-vertex list, 1..n entries per vertex
		std::vector<PrimitivesPerVertex>	_vertexEdgesLookup;		/// contains one entry per vertex
		std::vector<TriangleVertex>			_triangleVertices;		/// triangles-per-vertex list, 1..n entries per vertex
		std::vector<PrimitivesPerVertex>	_vertexTrianglesLookup;	/// contains one entry per vertex
		std::vector<TriangleEdges>			_triangleEdges;			/// the list of edges-per-triangle, 1 struct per triangle

		float radius;	/// maximum distance from origin (useful for QGLViewer)

		BoundingBox	bb;

		BoundingCube bc;

		BoundingCube lbc;


		/// functions
		virtual void GenerateEdges();

		void GenerateTriangleEdges();

		void GenerateEdgeMap();

		void GenerateTriangleMap();

		void GenerateBoundingBox(const std::vector<Vec3f>& vertices_);

	public:
		/// Constructors
		TriangleTopology();

		TriangleTopology(const std::vector<Vec3f>& vertices_, const std::vector<Triangle>& triangles_, const bool complete_ = false);

		~TriangleTopology();

		virtual void Clear();

		/**
		 * Initializes the triangle-mesh from a list of vertices and a list of triangle-indices.
		 * The fullUpdate flag indicates if the initialization should re-generate all data-structures.
		 * The default for fullUpdate_ is to only update vertices and triangle-indices. All other
		 * lists will be created when they are being accessed for the first time.
		 */
		virtual void Init(const std::vector<Vec3f>& vertices_, const std::vector<Triangle>& triangles_, const bool fullUpdate_ = false);

		/**
		 * Set Edges separately (e.g. when Edges have been generated from the tetrahedra already.
		 * This will also clear any edge-related mappings.
		 */
		void SetEdges(const std::vector<Edge>& edges_);

		/// accessor functions
		const std::vector<Vec3f>& GetVertices() const
		{
			return _vertices;
		}

		std::vector<Vec3f>& GetVertices()
		{
			return _vertices;
		}

		std::vector<Edge>& GetEdges()
		{
			if (_edges.size() == 0)
				GenerateEdges();
			return _edges;
		}

		const std::vector<Triangle>& GetTriangles() const
		{
			return _triangles;
		}

		std::vector<Triangle>& GetTriangles()
		{
			return _triangles;
		}

		const std::vector<TriangleVertex>& GetTrianglesPerVertices()
		{
			if (_triangleVertices.size() == 0)
				GenerateTriangleMap();
			return _triangleVertices;
		}

		const std::vector<EdgeVertex>& GetEdgesPerVertices()
		{
			if (_edgeVertices.size() == 0)
				GenerateEdgeMap();
			return _edgeVertices;
		}

		const std::vector<PrimitivesPerVertex>& GetEdgesPerVertexLookupTable()
		{
			if (_vertexEdgesLookup.size() == 0)
				GenerateEdgeMap();
			return _vertexEdgesLookup;
		}

		const std::vector<PrimitivesPerVertex>& GetTrianglesPerVertexLookupTable()
		{
			if (_vertexTrianglesLookup.size() == 0)
				GenerateTriangleMap();
			return _vertexTrianglesLookup;
		}

		const std::vector<TriangleEdges>& GetTriangleEdges()
		{
			if (_triangleEdges.size() == 0)
				GenerateTriangleEdges();
			return _triangleEdges;
		}

		const unsigned int GetNumVertices()
		{
			return _vertices.size();
		}

		const unsigned int GetNumEdges()
		{
			return _edges.size();
		}

		const unsigned int GetNumTriangles()
		{
			return _triangles.size();
		}

		const std::vector<Vec3f>& GetNormals()
		{
			if (_normals.size() == 0)
				GenerateNormals();
			return _normals;
		}

		void InvertNormals()
		{
			for (unsigned int i=0; i<_normals.size(); ++i)
			{
				_normals[i] = _normals[i] * -1.0f;
			}
		}

		void GenerateNormals();

		/**
		 * Find the Edge that is constructed by indices i0 and i1
		 */
		const unsigned int FindEdgeByIndex(const unsigned int i0, const unsigned int i1);

		float GetRadius()
		{
			return radius;
		}

		BoundingBox GetBoundingBox()
		{
			return bb;
		}

		/**
		 * Returns the bounding cube for the current mesh
		 */
		BoundingCube GetBoundingCube()
		{
			return bc;
		}

		/**
		 * Returns the larger bounding cube for the current mesh.
		 * This one will be used for the BCC (body centered cubic) and the octree.
		 * The size of this cube will be the original bounding cube's size  + 1.
		 */
		BoundingCube GetLargeBoundingCube()
		{
			return lbc;
		}

        void GenerateBoundingBox();

	};
}	/// end namespace TetraTools
