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

class DLL_EXPORT Octree {
private:
  OctreeNode *_root;
  unsigned int _maxDepth;
  std::vector<Vec3f>
      *_inPoints; // surface points for which we want to build the Octree
  std::vector<Triangle> *_inTris; // surface triangle indices
  Vec3f _minBC;
  Vec3f _maxBC;

  void generateBoundingCube();

public:
  /// The optional cancel flag may be set from another thread to stop the
  /// construction early (the octree is incomplete then).
  /// Only one octree may be constructed at a time (OctreeNode uses static
  /// members during the construction).
  Octree(const unsigned int maxDepth_, std::vector<Vec3f> *inPoints_,
         std::vector<Triangle> *inTris_,
         const std::atomic<bool> *cancel_ = nullptr);
  ~Octree();

  OctreeNode *getRootNode();

  unsigned int getDepth() const { return _maxDepth; }
};
