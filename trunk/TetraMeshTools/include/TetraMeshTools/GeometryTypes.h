/*
 * GeometryTypes.h
 *
 *  Created on: Aug 20, 2011
 *      Author: Dennis Luebke
 *
 * Type definitions for 3D-geometry.
 */

#ifndef GEOMETRYTYPES_H_
#define GEOMETRYTYPES_H_

#include "Vec2f.h"
#include "Vec3f.h"
#include "Vec4f.h"

	/**
	 * Struct for an axis-aligned bounding box (AABB).
	 * Contains the min point (min x, min y, minz) and max point (max x, max y, max z)
	 */
	struct BoundingBox
	{
		Vec3f min;
		Vec3f max;
	};

	/**
	 * Struct for an axis-aligned bounding cube as a basis for an octree and BCC (Body Centered Cubic).
	 * Contains the size that will be used for width, height and length.
	 */
	struct BoundingCube
	{
		float size;
		Vec3f center;
	};

	/**
	 * This class represents an edge and contains two vertex-indices
	 * in the vertices-list.
	 */
	class Edge
	{
	public:

		Edge()
		{}

		Edge(const unsigned int i0, const unsigned int i1)
		{
			index[0] = i0;
			index[1] = i1;
		}

		unsigned int index[2];

		bool operator ==(const Edge& e) const
		{
			return ((index[0] == e.index[0]) && (index[1] == e.index[1]));
		}

		/**
		 * Override operator < for use in std::map
		 */
		bool operator <(const Edge& e) const
		{
			for (unsigned int i=0; i<2; ++i)
			{
				if (index[i] < e.index[i])
					return true;
				else if (index[i] > e.index[i])
					return false;
			}
			return false;
		}

		friend std::ostream& operator<<( std::ostream& out, const Edge& e)
		{
			out<<e.index[0]<<"/"<<e.index[1];
			return out;
		}

		void Set(const unsigned int i0, const unsigned int i1)
		{
			index[0] = i0;
			index[1] = i1;
		}
	};

	/**
	 * This class represents a triangle and contains three vertex-indices
	 * in the vertices-list.
	 */
	class Triangle
	{
	public:
		unsigned int index[3];

		Triangle()
		{}

		Triangle(const unsigned int i0, const unsigned int i1, const unsigned int i2)
		{
			index[0] = i0;
			index[1] = i1;
			index[2] = i2;
		}

		static Vec3f GetNormal(const Vec3f& v0, const Vec3f& v1, const Vec3f& v2)
		{
			Vec3f d0 = v1 - v0;
			Vec3f d1 = v2 - v0;
			Vec3f normal = d0.cross(d1);
			normal.normalize();
			return normal;
		}

		inline bool operator ==(const Triangle& t) const
		{
			return ((index[0] == t.index[0]) && (index[1] == t.index[1]) && (index[2] == t.index[2]));
		}

		/**
		 * Override operator < for use in std::map
		 */
		inline bool operator <(const Triangle& t) const
		{
			for (unsigned int i=0; i<3; ++i)
			{
				if (index[i] < t.index[i])
					return true;
				else if (index[i] > t.index[i])
					return false;
			}
			return false;
		}

		friend std::ostream& operator<<( std::ostream& out, const Triangle& t)
		{
			out<<t.index[0]<<"/"<<t.index[1]<<"/"<<t.index[2];
			return out;
		}

		/**
		 * Returns the triangle's centroid using the actual vertex coordinates.
		 */
		static Vec3f Centroid(const Vec3f& p0, const Vec3f& p1, const Vec3f& p2)
		{
			Vec3f centroid = p0;
			centroid += p1;
			centroid += p2;
			centroid /= 3;
			return centroid;
		}
	};

	/**
	 * This class represents a tetrahedron and contains four vertex-indices
	 * in the vertices-list.
	 */
	class Tetrahedron
	{
	public:
		unsigned int index[4];

		inline bool operator ==(const Tetrahedron& t) const
		{
			return ((index[0] == t.index[0]) && (index[1] == t.index[1]) && (index[2] == t.index[2]) && (index[3] == t.index[3]));
		}

		/**
		 * Override operator < for use in std::map
		 */
		inline bool operator <(const Tetrahedron& t) const
		{
			for (unsigned int i=0; i<4; ++i)
			{
				if (index[i] < t.index[i])
					return true;
				else if (index[i] > t.index[i])
					return false;
			}
			return false;
		}

		friend std::ostream& operator<<( std::ostream& out, const Tetrahedron& t)
		{
			out<<t.index[0]<<"/"<<t.index[1]<<"/"<<t.index[2]<<"/"<<t.index[3];
			return out;
		}

		/**
		 * Returns the tetrahdron's centroid.
		 */
		static Vec3f Centroid(const Vec3f& p0, const Vec3f& p1, const Vec3f& p2, const Vec3f& p3)
		{
			Vec3f centroid = p0;
			centroid += p1;
			centroid += p2;
			centroid += p3;
			centroid /= 4;
			return centroid;
		}
	};

#endif /* GEOMETRYTYPES_H_ */
