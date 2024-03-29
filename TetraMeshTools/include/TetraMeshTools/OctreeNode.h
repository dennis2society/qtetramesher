/**
 *	OctreeNode.h
 *
 *	Node implementation for the Octree.
 *
 *	Author: Dennis Luebke, 2013
 *
 */

#pragma once

#include "GeometryTypes.h"
#include "TetraToolsExports.h"
#include <vector>
// #include <vld.h>

struct PointQuadrant {
  int left;
  int back;
  int top;
  unsigned int result;
};

class DLL_EXPORT OctreeNode {
private:
  const OctreeNode *_parent;
  unsigned int _depth;           // current depth level of our octree
  static unsigned int _maxDepth; // static int for maximum depth of our octree
  std::vector<OctreeNode *> _nodes;   // a maximum of 8 children is possible
  static std::vector<Vec3f> *_points; // pointer to surface mesh points
  static std::vector<Triangle> *_tris;
  Vec3f _minBC;
  Vec3f _maxBC;
  int _quadrant;

  void buildNode();

  // tmp children for return....
  std::vector<OctreeNode *> _tmpChildren;

public:
  // only necessary for root node
  explicit OctreeNode(const unsigned int maxDepth_,
                      std::vector<Vec3f> *inPoints_, const Vec3f &minBC_,
                      const Vec3f &maxBC_, std::vector<Triangle> *inTris_);
  // this constructor is only for the child nodes
  explicit OctreeNode(const OctreeNode *parent_, const Vec3f &minBC_,
                      const Vec3f &maxBC_, const unsigned int depth_,
                      const int quadrant_);
  ~OctreeNode();

  /**
   *	Returns NULL if querying the root node
   */
  const OctreeNode *getParent() const;

  const std::vector<OctreeNode *> &getChildren() const;

  const Vec3f &getMinBC() const;

  const Vec3f &getMaxBC() const;

  // returns true if the current node has children
  bool hasChildren() const;

  // returns the smalles children i.e. the "leafs" as a recursively generated
  // list when leafsOnly is set to true, we will only return the smallest child
  // leafs, while ignoring the containing cubes.
  const std::vector<OctreeNode *> &getLeafs(const bool leafsOnly_ = false);

  unsigned int getDepth() const { return _depth; }

  unsigned int getQuadrant() const { return _quadrant; }
};
