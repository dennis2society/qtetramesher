/**
 *	Octree.cpp
 *
 *	Author: Dennis Luebke, 2013
 *
 */

#include "Octree.h"
#include <algorithm>
#include <limits>
#ifndef WIN32
#include <cfloat>
#include <math.h>
#else
#include <float.h>
#endif

Octree::Octree(const unsigned int maxDepth_, std::vector<Vec3f>* inPoints_, std::vector<Triangle>* inTris_) : _inPoints(inPoints_), _maxDepth(maxDepth_), _inTris(inTris_)
{
	generateBoundingCube();
	_root = new OctreeNode(maxDepth_, inPoints_, _minBC, _maxBC, inTris_);
}

Octree::~Octree()
{
	if (_root != NULL)
	{
		delete _root;
		_root = NULL;
	}
}

void Octree::generateBoundingCube()
{
	std::vector<Vec3f>::const_iterator it;
	/// generate Bounding Box
	std::cout<<"Octree: Generating cubic bounding volume ..."<<std::endl;
	BoundingBox b;
	b.min = Vec3f(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	b.max = Vec3f(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
	float dist = 0;
	for (it=_inPoints->begin(); it!=_inPoints->end(); ++it)
	{
		Vec3f v = *it;
		float pd = v.squaredLength();
		if (pd > dist)
		{
			dist = pd;
		}
		//_vertices.push_back(v);
		b.min.x = std::min<float>(b.min.x, v.x);
		b.max.x = std::max<float>(b.max.x, v.x);
		b.min.y = std::min<float>(b.min.y, v.y);
		b.max.y = std::max<float>(b.max.y, v.y);
		b.min.z = std::min<float>(b.min.z, v.z);
		b.max.z = std::max<float>(b.max.z, v.z);
	}
	float dx = b.max.x - b.min.x;
	float dy = b.max.y - b.min.y;
	float dz = b.max.z - b.min.z;
	float maxDist = (dx > dy) ? dx : dy;
	maxDist = (maxDist > dz) ? maxDist : dz;
	BoundingCube bc;
	bc.size = maxDist;
	bc.center = Vec3f(b.min.x + dx/2, b.min.y + dy/2, b.min.z + dz / 2);
	const float lbcModifier = maxDist * 0.1f; /// add 10% volume for the larger bounding cube
	BoundingCube lbc;
	lbc.size = maxDist + lbcModifier;
	lbc.center = Vec3f(b.min.x + dx/2, b.min.y + dy/2, b.min.z + dz / 2);
	//std::cout<<"dx/dy/dz: "<<dx<<"/"<<dy<<"/"<<dz<<"; bc: "<<maxDist<<std::endl;
	//std::cout<<"\t BBox: "<<b.min<<"; "<<b.max<<std::endl;
	float maxHalf = maxDist / 2.0f;
	_minBC = Vec3f(lbc.center.x - maxHalf, lbc.center.y - maxHalf, lbc.center.z - maxHalf);
	_maxBC = Vec3f(lbc.center.x + maxHalf, lbc.center.y + maxHalf, lbc.center.z + maxHalf);
	std::cout<<"\tBC: "<<_minBC<<" ; "<<_maxBC<<std::endl;
}

OctreeNode* Octree::getRootNode()
{
	if (_root != NULL)
	{
		return _root;
	}
	return NULL;
}