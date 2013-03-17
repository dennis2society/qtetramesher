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
	_parent = NULL;
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
	//for (unsigned int i=0; i<_depth; ++i)
	//{
	//	std::cout<<"\t";
	//}
	//std::cout<<"Octree: Destructing level "<<_depth<<", node: "<<_quadrant<<std::endl;
	for (unsigned int i=0; i<8; ++i)
	{
		if (_nodes[i] != NULL)
		{
			delete _nodes[i];
			_nodes[i] = NULL;
		}
	}
	_tmpChildren.clear();
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
			// left
			if ((p->x <= center.x) && (p->x >=_minBC.x))
			{
				pq.left = 1;
			}
			else if ((p->x >= center.x) && (p->x <=_maxBC.x))
			{
				pq.left = 0;
			}
			else
			{
				pq.left = -1;
			}
			//pq.left = ((p->x < center.x) && (p->x >=_minBC.x));
			if ((p->y >= _minBC.y) && (p->y <= center.y))
			{
				pq.top = 0;
			}
			else if ((p->y >= center.y) && (p->y <= _maxBC.y))
			{
				pq.top = 1;
			}
			else
			{
				pq.top = -1;
			}
			//pq.top = (p->y > center.y);
			if ((p->z >= _minBC.z) && (p->z <= center.z))
			{
				pq.back = 1;
			}
			else if ((p->z >= center.z) && (p->y <= _maxBC.z))
			{
				pq.back = 0;
			}
			else
			{
				pq.back = -1;
			}
			//pq.back = (p->z < center.z);
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
			if (pq.top == 1)
			{
				pq.result = 4;
			}
			else if (pq.top == 0)
			{
				pq.result = 0;
			}
			else 
			{
				pq.result = -1;
			}
			if (pq.back == 1)
			{
				pq.result += 2;
			}
			else if (pq.back == 0)
			{
				pq.result += 0;
			}
			else
			{
				pq.result = -1;
			}
			if (pq.left == 1)
			{
				pq.result += 1;
			}
			else if (pq.left == 0)
			{
				pq.result += 0;
			}
			else
			{
				pq.result = -1;
			}

			//inPointsList[pq.result].push_back(p);
			if (pq.result >= 0)
			{
				usedQuadrants[pq.result] = true;
			}
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

const bool OctreeNode::hasChildren() const
{
	for (unsigned int i=0; i<8; ++i)
	{
		if (_nodes[i] != NULL)
		{
			return true;
		}
	}
	return false;
}

const std::vector<OctreeNode*>& OctreeNode::getLeafs()
{
	_tmpChildren.clear();
	if (this->hasChildren())
	{
		for (unsigned int i=0; i<_nodes.size(); ++i)
		{
			if (_nodes[i] != NULL)
			{
				const std::vector<OctreeNode*>& childLeafs = _nodes[i]->getLeafs();
				_tmpChildren.insert(_tmpChildren.end(), childLeafs.begin(), childLeafs.end());
			}
		}
		return _tmpChildren;
	}
	else

	{
		_tmpChildren.push_back(this);
		return _tmpChildren;
	}
}