/*
 * QuartetTetraStuffing.h
 *
 *	This is a wrapper interface for generating a tetrahedral mesh
 *	from a surface mesh using Jonathan Shewchuk's Isosurface TetraStuffing
 *	algorithm ( http://www.cs.berkeley.edu/~jrs/papers/stuffing.pdf ).
 *	This implementation is taken from
 *https://github.com/crawforddoran/quartet
 *
 *  Created on: May 29, 2018
 *      Author: Dennis Luebke
 */

#ifndef QUARTET_TETRA_STUFFING_H
#define QUARTET_TETRA_STUFFING_H

#include "TetraMeshTools/TetraToolsExports.h"
#include <vector>

/**
 * Simple temporary geometry structs to avoid naming clashes of
 * TetraMeshTools::Vec3f and the used tetra-meshing implementation. This
 * requires conversion to and from the temporary types to be used in an
 * application that uses a Vec3f/Vec4i type:
 * TetraMeshTools::Vec3f <-> tmpVec3f
 * TetraMeshTools::Triangle <-> inTriangle
 * TetraMeshTools::Tetrahedron <-> outTetra
 */
struct tmpVec3f {
  float x;
  float y;
  float z;
};

struct inTriangle {
  unsigned int index[3];
};

struct outTetra {
  unsigned int index[4];
};

class DLL_EXPORT QuartetTetraStuffing {
public:
  QuartetTetraStuffing() {}

  /**
   * Main function: Generate a tetrahedral mesh from a triangle surface
   * defined by a list of triangles and list of vertices.
   * Additionally we will set all necessary parameters for this
   * implementation of Shewchuk's algorithm. Also the output data containers will
   * be filled here after processing.
   */
  void GenerateFromSurface(const std::vector<inTriangle> &tris,
                           const std::vector<tmpVec3f> &verts,
                           float gridSpacing, float angleThreshold,
                           bool optimize);

  /**
   * Returns the generated tetrahedra vertices
   * This is using a temporary Vec3f type to avoid conflicts with Crawford's
   * vec.h declarations! This must be converted back to TetraMeshTools::Vec3f
   */
  std::vector<tmpVec3f> &GetTetraVertices();

  /**
   * Returns the generated tetrahedra tetra indices
   * This is using a temporary Tetrahedron type to avoid conflicts with
   * Crawford's vec.h declarations! This must be converted back to
   * TetraMeshTools::Tetrahedron
   */
  std::vector<outTetra> &GetTetras();

private:
  std::vector<tmpVec3f> tetraPoints;
  std::vector<outTetra> tetraIndices;
};

#endif // QUARTET_TETRA_STUFFING_H
