/*
 * FTetWildWrapper.cpp
 */

#include "FTetWildWrapper.h"

#include <floattetwild/FloatTetwild.h>
#include <floattetwild/Logger.hpp>
#include <floattetwild/Parameters.h>
#include <geogram/basic/common.h>
#include <geogram/mesh/mesh.h>

#include <cstdlib>
#include <stdexcept>

#ifdef FLOAT_TETWILD_USE_TBB
#include <tbb/global_control.h>
#endif

FTetWildWrapper::FTetWildWrapper() {}

FTetWildWrapper::~FTetWildWrapper() {}

void FTetWildWrapper::GenerateFromSurface(const std::vector<Triangle> &tris,
                                          const std::vector<Vec3f> &verts,
                                          double edgeLengthRel,
                                          double epsilonRel, double stopEnergy,
                                          bool coarsen) {
  tetraPoints.clear();
  tetraIndices.clear();
  if (verts.empty() || tris.empty())
    throw std::runtime_error("fTetWild: the surface mesh is empty.");

  static bool geogramInitialized = false;
  if (!geogramInitialized) {
#ifndef _WIN32
    setenv("GEO_NO_SIGNAL_HANDLER", "1", 1);
#endif
    GEO::initialize();
    geogramInitialized = true;
  }
  floatTetWild::Logger::init(true);

#ifdef FLOAT_TETWILD_USE_TBB
  // as in fTetWild's main(): deep recursions need a large thread stack
  tbb::global_control stackSize(tbb::global_control::thread_stack_size,
                                64 * 1024 * 1024);
#endif

  GEO::Mesh surface;
  surface.vertices.create_vertices(GEO::index_t(verts.size()));
  for (std::size_t i = 0; i < verts.size(); ++i) {
    double *p = surface.vertices.point_ptr(GEO::index_t(i));
    p[0] = verts[i].x;
    p[1] = verts[i].y;
    p[2] = verts[i].z;
  }
  surface.facets.create_triangles(GEO::index_t(tris.size()));
  for (std::size_t i = 0; i < tris.size(); ++i)
    for (GEO::index_t k = 0; k < 3; ++k)
      surface.facets.set_vertex(GEO::index_t(i), k, tris[i].index[k]);

  floatTetWild::Parameters params;
  params.ideal_edge_length_rel = edgeLengthRel;
  params.eps_rel = epsilonRel;
  params.stop_energy = stopEnergy;
  params.coarsen = coarsen;
  params.log_level = 3;

  Eigen::MatrixXd V;
  Eigen::MatrixXi T;
  if (floatTetWild::tetrahedralization(surface, params, V, T) != 0)
    throw std::runtime_error("fTetWild failed to tetrahedralize the surface.");

  tetraPoints.resize(std::size_t(V.rows()));
  for (Eigen::Index i = 0; i < V.rows(); ++i)
    tetraPoints[i] = Vec3f(float(V(i, 0)), float(V(i, 1)), float(V(i, 2)));
  tetraIndices.resize(std::size_t(T.rows()));
  for (Eigen::Index i = 0; i < T.rows(); ++i)
    for (int k = 0; k < 4; ++k) tetraIndices[i].index[k] = unsigned(T(i, k));
}
