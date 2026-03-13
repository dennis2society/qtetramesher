/*
 * TetgenWrapper.cpp
 *
 *  Created on: Oct 07, 2023
 *      Author: Dennis Luebke
 */

#include <TetgenWrapper.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <tetgen.h>
#include <cstring>
#include <cmath>

#ifdef _WIN32
#include <time.h>
#endif

/// local function because we don't want to expose Tetgen calls/data structures
void generateFacets(const std::vector<Triangle> &tris,
                    const std::vector<Vec3f> &verts, tetgenio &io) {
  tetgenio::facet *f;
  io.numberoffacets = tris.size();
  io.facetlist = new tetgenio::facet[tris.size()];
  io.facetmarkerlist = NULL;
  io.firstnumber = 0;
  for (auto i = 0; i < tris.size(); ++i) {
    f = &io.facetlist[i];
    f->numberofpolygons = 1;
    f->polygonlist = new tetgenio::polygon[f->numberofpolygons];
    f->numberofholes = 0;
    f->holelist = NULL;
    tetgenio::polygon *p = &f->polygonlist[0];
    p->numberofvertices = 3;
    p->vertexlist = new int[3];
    const Triangle t = tris.at(i);
    p->vertexlist[0] = t.index[0];
    p->vertexlist[1] = t.index[1];
    p->vertexlist[2] = t.index[2];
  }
  for (auto i = 0; i < io.numberoffacets; ++i) {
    tetgenio::facet k = io.facetlist[i];
  }
}

TetgenWrapper::TetgenWrapper() : m_noSubDivide(false), m_usePLC(true), m_qualityBound(2.0f), m_cellSize(0.0f)
{}

void TetgenWrapper::GenerateFromSurface(const std::vector<Triangle> &tris,
                                        const std::vector<Vec3f> &verts,
                                        float qualityBounds,
                                        float cellSize,
                                        bool usePLC)
{
  tetraIndices.clear();
  tetraPoints.clear();

  m_usePLC = usePLC;
  m_qualityBound = qualityBounds;
  m_cellSize = cellSize;

  // Auto-compute cell size from bounding box diagonal when none given (<=0)
  if (m_cellSize <= 0.0f && !verts.empty()) {
    Vec3f bbMin = verts[0], bbMax = verts[0];
    for (const auto &v : verts) {
      if (v.x < bbMin.x) bbMin.x = v.x;
      if (v.y < bbMin.y) bbMin.y = v.y;
      if (v.z < bbMin.z) bbMin.z = v.z;
      if (v.x > bbMax.x) bbMax.x = v.x;
      if (v.y > bbMax.y) bbMax.y = v.y;
      if (v.z > bbMax.z) bbMax.z = v.z;
    }
    const float dx = bbMax.x - bbMin.x;
    const float dy = bbMax.y - bbMin.y;
    const float dz = bbMax.z - bbMin.z;
    const float diag = std::sqrt(dx * dx + dy * dy + dz * dz);
    m_cellSize = diag / 10.0f;
    std::cout << "Auto cell size (diag=" << diag << "): " << m_cellSize << std::endl;
  }

  tetgenio input;
  tetgenio output;
  output.firstnumber = 0;

  input.firstnumber = 0;
  input.numberofpoints = verts.size();
  input.numberofpointattributes = 0;
  input.pointlist = new double[verts.size() * 3];
  for (auto i = 0; i < input.numberofpoints; ++i) {
    input.pointlist[i * 3] = verts.at(i).x;
    input.pointlist[i * 3 + 1] = verts.at(i).y;
    input.pointlist[i * 3 + 2] = verts.at(i).z;
  }
  for (auto i = 0; i < input.numberofpoints; ++i) {
    std::cout << "P[" << i << "]: " << input.pointlist[i * 3] << "/" << input.pointlist[i * 3 + 1]
              << "/" << input.pointlist[i * 3 + 2] << std::endl;
  }
  input.pointattributelist = NULL;
  input.numberoftrifaces = 0;
  input.trifacemarkerlist = NULL;
  generateFacets(tris, verts, input);

  /// invoke Tegen tetrahedralize
  try {
    std::string switches = generateTetgenParamString();
    char *params = new char[switches.size() + 1];
    std::strcpy(params, switches.c_str());
    tetrahedralize(params, &input, &output);
    delete[] params;
  }
  catch (int e)
  {
    std::cerr<<"Error! Tetgen tetrahedralize failed..."<<std::endl;
  }
  std::cout << "Num. Tetgen Tetra Points: " << output.numberofpoints
            << std::endl;
  std::cout << "Num. Tetgen Tetras: " << output.numberoftetrahedra << std::endl;
  /// copy output
  for (auto i = 0; i < output.numberofpoints; ++i) {
    Vec3f p;
    p.x = output.pointlist[i * 3];
    p.y = output.pointlist[i * 3 + 1];
    p.z = output.pointlist[i * 3 + 2];
    tetraPoints.push_back(p);
  }
  for (auto i = 0; i < output.numberoftetrahedra; ++i) {
    Tetrahedron t;
    t.index[0] = output.tetrahedronlist[i * 4];
    t.index[1] = output.tetrahedronlist[i * 4 + 1];
    t.index[2] = output.tetrahedronlist[i * 4 + 2];
    t.index[3] = output.tetrahedronlist[i * 4 + 3];
    tetraIndices.push_back(t);
  }
}

TetgenWrapper::~TetgenWrapper() {
  tetraIndices.clear();
  tetraPoints.clear();
}

std::vector<Vec3f> &TetgenWrapper::GetTetraVertices() { return tetraPoints; }

std::vector<Tetrahedron> &TetgenWrapper::GetTetras() { return tetraIndices; }

bool TetgenWrapper::saveAsTetgen(const std::string path,
                                 const std::string baseName,
                                 const std::vector<Tetrahedron> &tetras,
                                 const std::vector<Vec3f> &verts) {
  return false;
}

std::string TetgenWrapper::generateTetgenParamString()
{
  // Volume of a regular tetrahedron with edge s: V = s^3 * sqrt(2) / 12
  const double volume = static_cast<double>(m_cellSize) *
                        static_cast<double>(m_cellSize) *
                        static_cast<double>(m_cellSize) *
                        0.11785113019775792;  // sqrt(2)/12
  std::stringstream ss;
  if (m_usePLC)
    ss << "p";
  ss << "q" << std::fixed << std::setprecision(6) << m_qualityBound;
  ss << "a" << std::fixed << std::setprecision(10) << volume;
  std::cout << "ParamString: " << ss.str() << std::endl;
  ss << "V";
  return ss.str();
}
