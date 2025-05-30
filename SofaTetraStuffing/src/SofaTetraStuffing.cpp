/*
 * SofaTetraStuffing.cpp
 *
 *  Created on: Nov 26, 2011
 *      Author: Dennis Luebke
 */

#include "SofaTetraStuffing.h"
#ifdef _WIN32
// This is required for the Windows build to avoid
// min/max macro collisions
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif
#include "sofa/component/engine/generate/MeshTetraStuffing.h"

using namespace sofa::component::engine::generate;

void SofaTetraStuffing::GenerateFromSurface(const std::vector<Triangle> &tris,
                                            const std::vector<Vec3f> &verts,
                                            const float size_, const float as_,
                                            const float al_,
                                            const bool snapToSurface_,
                                            const bool splitTetras_) {
  // initialize SOFA's MeshTetraStuffing class
  MeshTetraStuffing::SPtr sofaStuffer =
      sofa::core::objectmodel::New<MeshTetraStuffing>();
  // prepare data
  MeshTetraStuffing::SeqPoints vertData;
  MeshTetraStuffing::SeqTriangles triData;
  // copy by element for conversion from our data types to SOFA's types
  sofaStuffer->d_inputPoints.beginEdit();
  for (unsigned int i = 0; i < verts.size(); ++i) {
    const Vec3f &vOrg = verts[i];
    sofa::type::Vec<3, double> v(vOrg.x, vOrg.y, vOrg.z);
    vertData.push_back(v);
  }
  sofaStuffer->d_inputPoints.setValue(vertData);
  sofaStuffer->d_inputPoints.endEdit();
  sofaStuffer->d_inputTriangles.beginEdit();
  for (unsigned int i = 0; i < tris.size(); ++i) {
    const Triangle &orgTri = tris[i];
    sofa::core::topology::BaseMeshTopology::Triangle t(
        orgTri.index[0], orgTri.index[1], orgTri.index[2]);
    triData.push_back(t);
  }
  sofaStuffer->d_inputTriangles.setValue(triData);
  sofaStuffer->d_inputTriangles.endEdit();
  // set parameters
  sofaStuffer->d_size.beginEdit();
  MeshTetraStuffing::Real ts = (MeshTetraStuffing::Real)size_;
  sofaStuffer->d_size.setValue(ts);
  sofaStuffer->d_size.endEdit();
  // as the following section is working fine, we obviously don't need
  // SOFA's begin-/endEdit() calls for setting primitive type values
  sofaStuffer->d_alphaLong = al_;
  sofaStuffer->d_alphaShort = as_;
  sofaStuffer->d_bSplitTetrahedra = splitTetras_;
  sofaStuffer->d_bSnapPoints = snapToSurface_;
  // tetrahedrize....
  sofaStuffer->init();
  // re-convert SOFA's result data to our own data type
  MeshTetraStuffing::SeqTetrahedra tetras =
      sofaStuffer->d_outputTetrahedra.getValue();
  tetraIndices.clear();
  for (unsigned int i = 0; i < tetras.size(); ++i) {
    sofa::core::topology::BaseMeshTopology::Tetra tout = tetras.at(i);
    Tetrahedron t;
    t.index[0] = tout[0];
    t.index[1] = tout[1];
    t.index[2] = tout[2];
    t.index[3] = tout[3];
    tetraIndices.push_back(t);
  }
  MeshTetraStuffing::SeqPoints outVerts =
      sofaStuffer->d_outputPoints.getValue();
  tetraPoints.clear();
  for (unsigned int i = 0; i < outVerts.size(); ++i) {
    sofa::defaulttype::Vec3Types::Coord vout = outVerts.at(i);
    Vec3f v;
    v.x = vout[0];
    v.y = vout[1];
    v.z = vout[2];
    tetraPoints.push_back(v);
  }
}

std::vector<Vec3f> &SofaTetraStuffing::GetTetraVertices() {
  return tetraPoints;
}

std::vector<Tetrahedron> &SofaTetraStuffing::GetTetras() {
  return tetraIndices;
}
