/*
 * TetgenWrapper.cpp
 *
 *  Created on: Oct 07, 2023
 *      Author: Dennis Luebke
 */

#include <TetgenWrapper.h>
#include <tetgen.h>
#include <iostream>

#ifdef _WIN32
#include <time.h>
#endif

/// local function because we don't want to expose Tetgen calls/data structures
void generateFacets(const std::vector<Triangle> &tris, const std::vector<Vec3f> &verts, tetgenio &io)
{
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

void TetgenWrapper::GenerateFromSurface(const std::vector<Triangle> &tris,
                                        const std::vector<Vec3f> &verts,
                                        float qualityBounds,
                                        float volumeConstraint,
                                        bool usePLC)
{
    tetraIndices.clear();
    tetraPoints.clear();

    tetgenio input;
    tetgenio output;
    output.firstnumber = 0;

    input.firstnumber = 0;
    std::vector<double> points = convertVertsToTetgenPoints(verts);
    input.numberofpoints = verts.size();
    input.numberofpointattributes = 0;
    input.pointlist = new double[verts.size() * 3];
    for (auto i = 0; i < input.numberofpoints * 3; ++i) {
        input.pointlist[i] = points.at(i);
    }
    input.pointattributelist = NULL;
    std::vector<int> faces = convertTrisToTetgenFaces(tris);
    input.numberoftrifaces = tris.size();
    input.trifacelist = new int[tris.size() * 3];
    for (auto i = 0; i < input.numberoftrifaces * 3; ++i) {
        input.trifacelist[i] = faces.at(i);
    }
    input.trifacemarkerlist = NULL;
    generateFacets(tris, verts, input);

    /// invoke Tegen tetrahedralize
    tetrahedralize("pq1.414a0.01", &input, &output);
    std::cout << "Num. Tetgen Tetra Points: " << output.numberofpoints << std::endl;
    std::cout << "Num. Tetgen Tetras: " << output.numberoftetrahedra << std::endl;
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

const std::vector<double>
TetgenWrapper::convertVertsToTetgenPoints(const std::vector<Vec3f> &verts_) {
  std::vector<double> points;
  for (auto &v : verts_) {
    for (auto i = 0; i < 3; ++i) {
      points.push_back(v[i]);
    }
  }
  return points;
}

const std::vector<int>
TetgenWrapper::convertTrisToTetgenFaces(const std::vector<Triangle> &tris_) {
  std::vector<int> faces;
  for (auto &t : tris_) {
    for (auto i = 0; i < 3; ++i)
      faces.push_back(t.index[i]);
  }
  return faces;
}

bool TetgenWrapper::saveAsTetgen(const std::string path,
                                 const std::string baseName,
                                 const std::vector<Tetrahedron> &tetras,
                                 const std::vector<Vec3f> &verts) {
  return false;
}

