/*
 * QuartetTetraStuffing.cpp
 *
 *  Created on: May 29, 2018
 *      Author: Dennis Luebke
 */

#include "QuartetTetraStuffing.h"
#include "vec.h"

#include "make_tet_mesh.h"
#include "make_signed_distance.h"
#include "feature.h"
#include "trimesh.h"


void QuartetTetraStuffing::GenerateFromSurface(const std::vector<inTriangle>& tris, const std::vector<tmpVec3f>& verts, float gridSpacing, float angleThreshold, bool optimize)
{
  // convert from input data to the implementation's data types
  std::vector<Vec3i> surf_tri;
  std::vector<Vec3f> surf_x;
  for (unsigned int i=0; i<tris.size(); ++i)
  {
    inTriangle org = tris.at(i);
    Vec3i tri(org.index[0], org.index[1], org.index[2]);
    surf_tri.push_back(tri);
  }
  for (unsigned int i=0; i<verts.size(); ++i)
  {
    tmpVec3f org = verts.at(i);
    Vec3f vert(org.x, org.y, org.z);
    surf_x.push_back(vert);
  }
  // Find bounding box
  Vec3f xmin=surf_x[0], xmax=surf_x[0];
  for (size_t i=1; i<surf_x.size(); ++i)
      update_minmax(surf_x[i], xmin, xmax);

  // Build triangle mesh data structure from converted input data
  TriMesh trimesh(surf_x, surf_tri);

  // Make the level set
	
	// Determining dimensions of voxel grid.
	// Round up to ensure voxel grid completely contains bounding box.
	// Also add padding of 2 grid points around the bounding box.
	// NOTE: We add 5 here so as to add 4 grid points of padding, as well as
	// 1 grid point at the maximal boundary of the bounding box 
	// ie: (xmax-xmin)/dx + 1 grid points to cover one axis of the bounding box
	Vec3f origin=xmin-Vec3f(2*gridSpacing);
	int ni = (int)std::ceil((xmax[0]-xmin[0])/gridSpacing)+5;
  int nj = (int)std::ceil((xmax[1]-xmin[1])/gridSpacing)+5;
  int nk = (int)std::ceil((xmax[2]-xmin[2])/gridSpacing)+5;

  SDF sdf(origin, gridSpacing, ni, nj, nk); // Initialize signed distance field.
  std::printf("making %dx%dx%d level set\n", ni, nj, nk);
  make_signed_distance(surf_tri, surf_x, sdf);


  // Then the tet mesh
  TetMesh mesh;

  FeatureSet featureSet;
  featureSet.autoDetectFeatures(trimesh, angleThreshold);

  clock_t time = clock();

  // Make tet mesh without features
  std::printf("making tet mesh\n");
  make_tet_mesh(mesh, sdf, featureSet, optimize, false, true);
  std::printf("done tet mesh\n");

  time = clock() - time;
  std::printf("mesh generation took %f seconds.\n", 
              ((float)time)/CLOCKS_PER_SEC);

  // convert back to our own (temporary) format
  const std::vector<Vec4i>& tets = mesh.tets();
  for (unsigned int i=0; i<tets.size(); ++i)
  {
    outTetra t;
    t.index[0] = tets.at(i)[0];
    t.index[1] = tets.at(i)[1];
    t.index[2] = tets.at(i)[2];
    t.index[3] = tets.at(i)[3];
    tetraIndices.push_back(t);
  }
  const std::vector<Vec3f>& tVerts = mesh.verts();
  for (unsigned int i=0; i<tVerts.size(); ++i)
  {
    tmpVec3f v;
    v.x = tVerts.at(i)[0];
    v.y = tVerts.at(i)[1];
    v.z = tVerts.at(i)[2];
    tetraPoints.push_back(v);
  }
}

std::vector<tmpVec3f>& QuartetTetraStuffing::GetTetraVertices()
{
  return tetraPoints;
}

std::vector<outTetra>& QuartetTetraStuffing::GetTetras()
{
  return tetraIndices;
}
