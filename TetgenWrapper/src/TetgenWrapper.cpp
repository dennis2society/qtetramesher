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

    input.firstnumber = 0;
    std::vector<double> points = convertVertsToTetgenPoints(verts);
    input.numberofpoints = verts.size();
    input.numberofpointattributes = 0;
    input.pointlist = &points[0];
    // input.pointattributelist = nullptr;
    std::vector<int> faces = convertTrisToTetgenFaces(tris);
    input.numberoftrifaces = tris.size();
    input.trifacelist = &faces[0];
    input.trifacemarkerlist = nullptr;

    /// invoke Tegen tetrahedralize
    tetrahedralize("pq1.414a0.1", &input, &output);
    std::cout << "Num. Tetgen Tetra Points: " << output.numberofpoints << std::endl;
    std::cout << "Num. Tetgen Tetras: " << output.numberoftetrahedra << std::endl;

    //points.clear();
    //faces.clear();
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

