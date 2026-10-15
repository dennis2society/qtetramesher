/*
 * TriMeshLoader.cpp
 *
 *  Created on: Jan 04, 2013
 *      Author: Dennis Luebke
 */

#include "TetraMeshTools/TriMeshLoader.h"
#include <fstream>
#include <iostream>
// assimp "Asset Importer Library" - https://github.com/assimp/assimp
#include <assimp/Importer.hpp>  // C++ importer interface
#include <assimp/config.h>      // Post processing properties
#include <assimp/postprocess.h> // Post processing flags
#include <assimp/scene.h>       // Output data structure

TetraTools::TriMeshLoader::TriMeshLoader() {}

TetraTools::TriMeshLoader::~TriMeshLoader() {
  _vertices.clear();
  _triangles.clear();
}

bool TetraTools::TriMeshLoader::loadFile(const std::string &path_and_filename,
                                         bool repair) {
  _vertices.clear();
  _triangles.clear();
  _repairReport = MeshRepairReport();
  Assimp::Importer importer;
  // Only the positions are needed. Removing normals, texture coordinates etc.
  // lets JoinIdenticalVertices merge vertices by position, otherwise e.g. an
  // OBJ with sharp edges stays split there and is not closed.
  importer.SetPropertyInteger(
      AI_CONFIG_PP_RVC_FLAGS,
      aiComponent_NORMALS | aiComponent_TANGENTS_AND_BITANGENTS |
          aiComponent_COLORS | aiComponent_TEXCOORDS |
          aiComponent_BONEWEIGHTS | aiComponent_ANIMATIONS |
          aiComponent_TEXTURES | aiComponent_LIGHTS | aiComponent_CAMERAS |
          aiComponent_MATERIALS);
  // drop point and line primitives, only triangles are used
  importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE,
                              aiPrimitiveType_POINT | aiPrimitiveType_LINE);
  const aiScene *tm = importer.ReadFile(
      path_and_filename,
      aiProcess_RemoveComponent | aiProcess_Triangulate |
          aiProcess_JoinIdenticalVertices | aiProcess_SortByPType |
          aiProcess_PreTransformVertices | aiProcess_ValidateDataStructure);
  if (nullptr == tm) {
    std::cerr << "TrimeshLoader failed: " << importer.GetErrorString()
              << std::endl;
    return false;
  }
  std::cout << "Assimp: Num Meshes: " << tm->mNumMeshes << std::endl;
  // A file can contain several meshes (e.g. OBJ groups or one per material),
  // all of them belong to the surface. PreTransformVertices has already
  // moved them to their place in the scene.
  for (unsigned int m = 0; m < tm->mNumMeshes; ++m) {
    const aiMesh *mesh = tm->mMeshes[m];
    if (mesh == nullptr || !(mesh->mPrimitiveTypes & aiPrimitiveType_TRIANGLE))
      continue;
    const unsigned int offset = static_cast<unsigned int>(_vertices.size());
    for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
      const aiVector3D &p = mesh->mVertices[i];
      _vertices.push_back(Vec3f(p.x, p.y, p.z));
    }
    for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
      const aiFace &f = mesh->mFaces[i];
      if (f.mNumIndices != 3) continue;  // left over by Triangulate
      Triangle t;
      t.index[0] = offset + f.mIndices[0];
      t.index[1] = offset + f.mIndices[1];
      t.index[2] = offset + f.mIndices[2];
      _triangles.push_back(t);
    }
  }
  if (_vertices.empty() || _triangles.empty()) {
    std::cerr << "Error loading surface mesh! The file contains no triangles."
              << std::endl;
    _vertices.clear();
    _triangles.clear();
    return false;
  }
  std::cout << "Successfully loaded surface mesh with " << _vertices.size()
            << " vertices and " << _triangles.size() << " triangles."
            << std::endl;

  if (repair) {
    _repairReport = RepairSurfaceMesh(_vertices, _triangles);
    if (_repairReport.Changed())
      std::cout << "Surface mesh repaired: " << _repairReport.Summary()
                << std::endl;
    if (_repairReport.numBoundaryEdges || _repairReport.numNonManifoldEdges)
      std::cout << "Surface mesh still has "
                << _repairReport.numBoundaryEdges << " open and "
                << _repairReport.numNonManifoldEdges
                << " non-manifold edges." << std::endl;
    if (_triangles.empty()) {
      std::cerr << "Error loading surface mesh! No valid triangles left."
                << std::endl;
      _vertices.clear();
      return false;
    }
  }
  return true;
}
