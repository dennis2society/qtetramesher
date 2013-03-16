/**
 *	Octree.h
 *
 *	Octree implementation for 3D surface mesh subdivision.
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
	Vec3f						_minBC, _maxBC;

	void generateBoundingCube();


public:
	Octree(const unsigned int maxDepth_, std::vector<Vec3f>* inPoints_);
	~Octree();
	void clear();

	const OctreeNode* getRootNode() const;
	
};