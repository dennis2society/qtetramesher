/*
 * TetgenLoader.cpp
 *
 *  Created on: May 19, 2013
 *      Author: Dennis Luebke
 */

#include "TetraMeshTools/TetgenLoader.h"

#include <fstream>
#include <iostream>
#include <sstream>

namespace TetraTools {

TetgenLoader::TetgenLoader() {}

TetgenLoader::TetgenLoader(const std::string &fileName_) {
  Load(fileName_);
}

bool TetgenLoader::Load(const std::string &fileName_) {
  _vertices.clear();
  _tetraIndices.clear();
  _fileName = fileName_;
  ParseNodes();
  ParseElems();
  return !_vertices.empty() && !_tetraIndices.empty();
}

bool TetgenLoader::ParseFile(std::ifstream & /*file_*/) {
  // Not used: Load() handles both .node and .ele files directly.
  return true;
}

void TetgenLoader::ParseNodes() {
  std::string nodeFile = _fileName + ".node";
  std::ifstream f(nodeFile.c_str());
  if (!f.is_open()) {
    std::cerr << "TetgenLoader: Cannot open node file '" << nodeFile << "'."
              << std::endl;
    return;
  }

  std::string line;
  // Skip comment lines; first non-comment line is the header: count 3 [nattr] [nboundary]
  unsigned int count = 0;
  while (std::getline(f, line)) {
    if (line.empty() || line[0] == '#') continue;
    std::istringstream ss(line);
    int dim, nattr, nbnd;
    ss >> count >> dim >> nattr >> nbnd;
    break;
  }

  _vertices.reserve(count);
  unsigned int read = 0;
  while (read < count && std::getline(f, line)) {
    if (line.empty() || line[0] == '#') continue;
    std::istringstream ss(line);
    unsigned int idx;
    float x, y, z;
    ss >> idx >> x >> y >> z;
    _vertices.push_back(Vec3f(x, y, z));
    ++read;
  }
  f.close();
  std::cout << "TetgenLoader: Loaded " << _vertices.size() << " vertices from '"
            << nodeFile << "'." << std::endl;
}

void TetgenLoader::ParseElems() {
  std::string eleFile = _fileName + ".ele";
  std::ifstream f(eleFile.c_str());
  if (!f.is_open()) {
    std::cerr << "TetgenLoader: Cannot open ele file '" << eleFile << "'."
              << std::endl;
    return;
  }

  std::string line;
  // Skip comment lines; first non-comment line is the header: count 4 [nattr]
  unsigned int count = 0;
  while (std::getline(f, line)) {
    if (line.empty() || line[0] == '#') continue;
    std::istringstream ss(line);
    int nodesPerTet, nattr;
    ss >> count >> nodesPerTet >> nattr;
    break;
  }

  _tetraIndices.reserve(count);
  unsigned int read = 0;
  while (read < count && std::getline(f, line)) {
    if (line.empty() || line[0] == '#') continue;
    std::istringstream ss(line);
    unsigned int idx, i0, i1, i2, i3;
    ss >> idx >> i0 >> i1 >> i2 >> i3;
    // Tetgen uses 1-based indices; convert to 0-based
    Tetrahedron t;
    t.index[0] = i0 - 1;
    t.index[1] = i1 - 1;
    t.index[2] = i2 - 1;
    t.index[3] = i3 - 1;
    _tetraIndices.push_back(t);
    ++read;
  }
  f.close();
  std::cout << "TetgenLoader: Loaded " << _tetraIndices.size()
            << " tetrahedra from '" << eleFile << "'." << std::endl;
}

void TetgenLoader::ParseTetgen(std::ifstream & /*file_*/) {
  // Retained for ABI compatibility; parsing is done via ParseNodes/ParseElems.
}

const std::vector<Vec3f> &TetgenLoader::GetVertices() const {
  return _vertices;
}

const std::vector<Tetrahedron> &TetgenLoader::GetTetras() const {
  return _tetraIndices;
}

bool TetgenLoader::IsLoaded() {
  return !_vertices.empty() && !_tetraIndices.empty();
}

void TetgenLoader::Clear() {
  _vertices.clear();
  _tetraIndices.clear();
  _fileName.clear();
}

}  // namespace TetraTools
