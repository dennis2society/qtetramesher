/*
 * TriMeshLoader.cpp
 *
 *  Created on: Jan 04, 2013
 *      Author: Dennis Luebke
 */

#include "TetraMeshTools/TriMeshLoader.h"
#include <fstream>
// assimp "Asset Importer Library" - https://github.com/assimp/assimp
#include <assimp/Importer.hpp>  // C++ importer interface
#include <assimp/postprocess.h> // Post processing flags
#include <assimp/scene.h>       // Output data structure

TetraTools::TriMeshLoader::TriMeshLoader() {}

TetraTools::TriMeshLoader::~TriMeshLoader() {
  _vertices.clear();
  _triangles.clear();
}

bool TetraTools::TriMeshLoader::loadFile(const std::string &path_and_filename) {
  _vertices.clear();
  _triangles.clear();
  Assimp::Importer importer;
  const aiScene *tm =
      importer.ReadFile(path_and_filename, aiProcess_Triangulate |
                                               aiProcess_JoinIdenticalVertices |
                                               aiProcess_SortByPType);
  if (nullptr == tm) {
    std::cerr << "TrimeshLoader failed... " << std::endl;
    return false;
  }
  std::cout << "Assimp: Num Meshes: " << tm->mNumMeshes << std::endl;
  const aiMesh *mesh = tm->mMeshes[0];
  if (mesh->mNumVertices == 0 || mesh->mNumFaces == 0) {
    std::cerr << "Error loading surface mesh! Vertices or faces are empty..."
              << std::endl;
    return false;
  } else {
    std::cout << "Successfully loaded surface mesh with " << mesh->mNumVertices
              << " vertices and " << mesh->mNumFaces << " faces." << std::endl;
  }
  for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
    aiVector3t<float> p = mesh->mVertices[i];
    Vec3f v(p[0], p[1], p[2]);
    _vertices.push_back(v);
  }
  for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
    const aiFace &f = mesh->mFaces[i];
    if (f.mNumIndices != 3) {
      std::cerr << "Error in TriMeshLoading! Loaded shape has more than three "
                   "indices!"
                << std::endl;
      return false;
    }
    Triangle t;
    t.index[0] = f.mIndices[0];
    t.index[1] = f.mIndices[1];
    t.index[2] = f.mIndices[2];
    _triangles.push_back(t);
  }
  // delete mesh;
  // delete tm;
  return true;
}
