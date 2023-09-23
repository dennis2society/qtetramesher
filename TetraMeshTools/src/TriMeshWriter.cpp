/*
 * TriMeshWriter.cpp
 *
 *  Created on: Feb 09, 2013
 *      Author: Dennis Luebke
 */

#include "TetraMeshTools/TriMeshWriter.h"
#include "TriMesh.h"
#include <assimp/Exporter.hpp>
#include <assimp/cexport.h>
#include <assimp/postprocess.h> // Post processing flags
#include <assimp/scene.h>       // Output data structure
#include <fstream>
#include <string>

TetraTools::TriMeshWriter::TriMeshWriter() {}

TetraTools::TriMeshWriter::~TriMeshWriter() {}

// This is an internal function to translate the QFileDialog's
// selected extension into a file ending for the assimp exporter
const std::string
translateFileExtensionToAssimpFormatId(const std::string &extension) {
  if (extension.compare("obj") == 0) {
    return "objnomtl";
  }
  if (extension.compare("dae") == 0) {
    return "collada";
  }
  if (extension.compare("stl") == 0) {
    return "stl";
  }
  if (extension.compare("3ds") == 0) {
    return "3ds";
  }
  if (extension.compare("ply") == 0) {
    return "ply";
  }
  return "objnomtl";
}

bool TetraTools::TriMeshWriter::writeFile(
    const std::string &path_and_filename, const std::string &extension_,
    const std::vector<Vec3f> &verts_, const std::vector<Triangle> &triangles_) {
  // This was done with help from here:
  // https://github.com/assimp/assimp/issues/203#issuecomment-817188468

  // copy vertices
  aiVector3D *verts = new aiVector3D[verts_.size()];
  for (auto i = 0; i < verts_.size(); ++i) {
    verts[i] = aiVector3D();
    verts[i].Set(verts_.at(i).x, verts_.at(i).y, verts_.at(i).z);
  }

  // copy triangles
  aiFace *faces = new aiFace[triangles_.size()];
  for (auto i = 0; i < triangles_.size(); ++i) {
    faces[i].mNumIndices = 3;
    faces[i].mIndices = new unsigned int[faces[i].mNumIndices];
    for (auto n = 0; n < 3; ++n) {
      faces[i].mIndices[n] = triangles_.at(i).index[n];
    }
  }

  aiMesh *mesh = new aiMesh();
  mesh->mNumVertices = verts_.size();
  mesh->mVertices = verts;
  mesh->mNumFaces = triangles_.size();
  mesh->mFaces = faces;
  mesh->mPrimitiveTypes = aiPrimitiveType_TRIANGLE;

  // we need a valid material, even if empty:
  aiMaterial *material = new aiMaterial();

  // now we need a root node
  aiNode *root = new aiNode();
  root->mNumMeshes = 1;
  root->mMeshes = new unsigned[]{0};

  // pack mesh(es), material, and root node into a new minimal aiScene
  aiScene *out = new aiScene();
  out->mNumMeshes = 1;
  out->mMeshes = new aiMesh *[] { mesh };
  out->mNumMaterials = 1;
  out->mMaterials = new aiMaterial *[] { material };
  out->mRootNode = root;
  out->mMetaData = new aiMetadata();

  // Export the mesh
  Assimp::Exporter exporter;
  if (exporter.Export(out, translateFileExtensionToAssimpFormatId(extension_), path_and_filename)
      != AI_SUCCESS) {
    std::cerr << exporter.GetErrorString() << " : " << extension_ << std::endl;
    delete out;
    return false;
  }

  // deleting the scene will also take care of the vertices, faces, meshes,
  // materials, nodes, etc.
  delete out;
  return true;
}
