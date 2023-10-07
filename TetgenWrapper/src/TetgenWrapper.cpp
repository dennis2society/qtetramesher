/*
 * TetgenWrapper.cpp
 *
 *  Created on: Oct 07, 2023
 *      Author: Dennis Luebke
 */

#include <TetgenWrapper.h>
#include <iostream>
#include <sstream>
#include <tetgen.h>

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
}

TetgenWrapper::TetgenWrapper() : m_noSubDivide(false), m_usePLC(true), m_qualityBound(1.5f), m_volumeConstraint(0.3f)
{}

void TetgenWrapper::GenerateFromSurface(const std::vector<Triangle> &tris,
                                        const std::vector<Vec3f> &verts,
                                        float qualityBounds,
                                        float volumeConstraint,
                                        bool usePLC)
{
  tetraIndices.clear();
  tetraPoints.clear();

  m_usePLC = usePLC;
  m_qualityBound = qualityBounds;
  m_volumeConstraint = volumeConstraint;

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
  input.pointattributelist = NULL;
  input.numberoftrifaces = tris.size();
  input.trifacelist = new int[tris.size() * sizeof(Triangle)];
  for (auto i = 0; i < input.numberoftrifaces; ++i) {
    input.trifacelist[i * 3] = tris.at(i).index[0];
    input.trifacelist[i * 3 + 1] = tris.at(i).index[1];
    input.trifacelist[i * 3 + 2] = tris.at(i).index[2];
  }
  input.trifacemarkerlist = NULL;
  generateFacets(tris, verts, input);

  /// invoke Tegen tetrahedralize
  char* params = strdup(generateTetgenParamString().c_str());
  tetrahedralize(params, &input, &output);
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
  delete[] params;
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
  std::stringstream ss;
  if (m_usePLC)
    ss << "p";
  ss << "q" << m_qualityBound;
  ss << "a" << m_volumeConstraint;
  std::cout << "ParamString: " << ss.str() << std::endl;
  return ss.str();

}
