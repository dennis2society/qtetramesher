/**
 *	Octree.h
 *
 *	Octree implementation for 3D surface mesh subdivision.
 *	Each Octree node that intersects a surface mesh triangle will be used.
 *
 *	Author: Dennis Luebke, 2013
 *
 */

#pragma once

#include "OctreeNode.h"

class DLL_EXPORT Octree
{
private:
	OctreeNode*					_root;
	const unsigned int			_maxDepth;
	const std::vector<Vec3f>*	_inPoints;	// surface points for which we want to build the Octree
	const std::vector<Triangle>* _inTris;	// surface triangle indices
	Vec3f						_minBC, _maxBC;

	void generateBoundingCube();


public:
	Octree(const unsigned int maxDepth_, std::vector<Vec3f>* inPoints_, std::vector<Triangle>* inTris_);
	~Octree();

	OctreeNode* getRootNode();

	const unsigned int getDepth() const
	{
		return _maxDepth;
	}
	
};