/**
 *	OctreeNode.cpp
 *
 *	Author: Dennis Luebke, 2013
 *
 */

#include "OctreeNode.h"
#include <iostream>

std::vector<Vec3f>* OctreeNode::_points = NULL;
unsigned int OctreeNode::_maxDepth = 0;

OctreeNode::OctreeNode(const unsigned int maxDepth_, std::vector<Vec3f>* inPoints_, const Vec3f& minBC_, const Vec3f& maxBC_) : _minBC(minBC_), _maxBC(maxBC_), _quadrant(-1)
{
	OctreeNode::_points = inPoints_;
	OctreeNode::_maxDepth = maxDepth_;
	_depth = 0;
	_nodes.resize(8);
	for (unsigned int i=0; i<8; ++i)
	{
		_nodes[i] = NULL;
	}
	buildNode();
}
	
OctreeNode::OctreeNode(const OctreeNode* parent_, const Vec3f& minBC_, const Vec3f& maxBC_, const unsigned int depth_, const int quadrant_) : _parent(parent_), _minBC(minBC_), _maxBC(maxBC_), _depth(depth_), _quadrant(quadrant_)
{
	_nodes.resize(8);
	for (unsigned int i=0; i<8; ++i)
	{
		_nodes[i] = NULL;
	}
	buildNode();
}

OctreeNode::~OctreeNode()
{
	clear();
}

const OctreeNode* OctreeNode::getParent() const
{
	return _parent;
}

const std::vector<OctreeNode*>& OctreeNode::getChildren() const
{
	return _nodes;
}

const Vec3f& OctreeNode::getMinBC() const
{
	return _minBC;
}

const Vec3f& OctreeNode::getMaxBC() const
{
	return _maxBC;
}

void OctreeNode::clear()
{
	while (!_nodes.empty())
	{
		OctreeNode* on = _nodes.back();
		if (on != NULL)
		{
			on->clear();
			delete on;
		}
		_nodes.pop_back();
	}
	//_pointsInSelf.clear();
}

void OctreeNode::buildNode()
{
	if (_depth < OctreeNode::_maxDepth)
	{
		//std::vector<std::vector<Vec3f*> > inPointsList;
		//inPointsList.resize(8);
		// prepare 8 possible children
		Vec3f center = _minBC + _maxBC;
		center /= 2.0f;
		bool usedQuadrants[8];
		for (unsigned int i=0; i<8; ++i)
		{
			usedQuadrants[i] = false;
		}
		for (unsigned int i=0; i<_points->size(); ++i)
		{
			PointQuadrant pq;
			Vec3f* p = &(_points->at(i));
			//_pointsInSelf.push_back(p);
			// determine child quadrant
			pq.left = (p->x < center.x);
			pq.top = (p->y > center.y);
			pq.back = (p->z < center.z);
			// determine quadrant index 
			// the sub-volumes will be ordered as followed: (ordered by priority)
			// top->bottom; back->front; left->right;
            //      +   -   +   -   +
            //     /   0   /   1   /|
            //    +   -   +   -   + |   
            //   /   2   /   3   /|1|
            //  +   -   +   -   + | +
            //  |       |       |3|/| 
			//  |   2   |   3   | + |
            //  |       |       |/|5|
			//  +   -   +   -   + | +
            //  |       |       |7|/ 
			//  |   6   |   7   | +
            //  |       |       |/
			//  +   -   +   -   +
			pq.result = (pq.top) ? 0 : 4;
			pq.result += (pq.back) ? 0 : 2;
			pq.result += (pq.left) ? 0 : 1;
			//inPointsList[pq.result].push_back(p);
			usedQuadrants[pq.result] = true;
		}
		// Add the 8 nodes to our child list.
		// Ignore the qudrant if empty i.e. if it has no points
		//std::cout<<"\tBC center["<<_depth<<"]["<<_quadrant<<"] : "<<center<<std::endl;
		if (usedQuadrants[0])
		{
			OctreeNode* on0 = new OctreeNode(this, Vec3f(_minBC.x, center.y, _minBC.z), Vec3f(center.x, _maxBC.y, center.z), _depth+1, 0);
			_nodes[0] = on0;
		}
		if (usedQuadrants[1])
		{
			OctreeNode* on1 = new OctreeNode(this, Vec3f(center.x, center.y, _minBC.z), Vec3f(_maxBC.x, _maxBC.y, center.z), _depth+1, 1);
			_nodes[1] = on1;
		}
		if (usedQuadrants[2])
		{
			OctreeNode* on2 = new OctreeNode(this, Vec3f(_minBC.x, center.y, center.z), Vec3f(center.x, _maxBC.y, _maxBC.z), _depth+1, 2);
			_nodes[2] = on2;
		}
		if (usedQuadrants[3])
		{
			OctreeNode* on3 = new OctreeNode(this, Vec3f(center.x, center.y, center.z), Vec3f(_maxBC.x, _maxBC.y, _maxBC.z), _depth+1, 3);
			_nodes[3] = on3;
		}
		// lower quadrants
		if (usedQuadrants[4])
		{
			OctreeNode* on4 = new OctreeNode(this, Vec3f(_minBC.x, _minBC.y, _minBC.z), Vec3f(center.x, center.y, center.z), _depth+1, 4);
			_nodes[4] = on4;
		}
		if (usedQuadrants[5])
		{
			OctreeNode* on5 = new OctreeNode(this, Vec3f(center.x, _minBC.y, _minBC.z), Vec3f(_maxBC.x, center.y, center.z), _depth+1, 5);
			_nodes[5] = on5;
		}
		if (usedQuadrants[6])
		{
			OctreeNode* on6 = new OctreeNode(this, Vec3f(_minBC.x, _minBC.y, center.z), Vec3f(center.x, center.y, _maxBC.z), _depth+1, 6);
			_nodes[6] = on6;
		}
		if (usedQuadrants[7])
		{
			OctreeNode* on7 = new OctreeNode(this, Vec3f(center.x, _minBC.y, center.z), Vec3f(_maxBC.x, center.y, _maxBC.z), _depth+1, 7);
			_nodes[7] = on7;
		}
	}
}
