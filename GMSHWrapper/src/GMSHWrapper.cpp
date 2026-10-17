/*
 * GMSHWrapper.cpp
 *
 * Wrapper for generating a tetrahedral mesh from a closed triangle surface
 * mesh using the GMSH library (https://gmsh.info/).
 *
 *  Created on: 2024
 *      Author: Dennis Luebke
 */

#include <GMSHWrapper.h>
#include <gmsh.h>
#include <cmath>
#include <iostream>
#include <map>
#include <stdexcept>

GMSHWrapper::GMSHWrapper() {}

GMSHWrapper::~GMSHWrapper() {
  tetraPoints.clear();
  tetraIndices.clear();
}

void GMSHWrapper::GenerateFromSurface(const std::vector<Triangle> &tris,
                                      const std::vector<Vec3f> &verts,
                                      double meshSizeMin,
                                      double meshSizeMax,
                                      int algorithm3D) {
  tetraPoints.clear();
  tetraIndices.clear();

  if (verts.empty() || tris.empty()) {
    throw std::runtime_error("GMSH: the surface mesh is empty.");
  }
  // Frontal (4) needs a GMSH build with Netgen, so only these are supported
  if (algorithm3D != 1 && algorithm3D != 10) {
    throw std::runtime_error("GMSH: unsupported 3D algorithm " +
                             std::to_string(algorithm3D) +
                             " (use 1 = Delaunay or 10 = HXT).");
  }
  for (const Triangle &t : tris) {
    for (int k = 0; k < 3; ++k) {
      if (t.index[k] >= verts.size()) {
        throw std::runtime_error(
            "GMSH: the surface mesh has invalid vertex indices.");
      }
    }
  }

  // Auto-compute mesh size from bounding-box diagonal when not specified.
  if (meshSizeMax <= 0.0 || meshSizeMin <= 0.0) {
    Vec3f bbMin = verts[0], bbMax = verts[0];
    for (const auto &v : verts) {
      if (v.x < bbMin.x) bbMin.x = v.x;
      if (v.y < bbMin.y) bbMin.y = v.y;
      if (v.z < bbMin.z) bbMin.z = v.z;
      if (v.x > bbMax.x) bbMax.x = v.x;
      if (v.y > bbMax.y) bbMax.y = v.y;
      if (v.z > bbMax.z) bbMax.z = v.z;
    }
    const double dx = bbMax.x - bbMin.x;
    const double dy = bbMax.y - bbMin.y;
    const double dz = bbMax.z - bbMin.z;
    const double diag = std::sqrt(dx * dx + dy * dy + dz * dz);
    if (meshSizeMax <= 0.0)
      meshSizeMax = diag / 10.0;
    if (meshSizeMin <= 0.0)
      meshSizeMin = meshSizeMax / 10.0;
    std::cout << "GMSHWrapper: auto mesh size min=" << meshSizeMin
              << " max=" << meshSizeMax << " (diag=" << diag << ")"
              << std::endl;
  }
  usedMeshSizeMin = meshSizeMin;
  usedMeshSizeMax = meshSizeMax;

  // Whether a patch can be parametrized depends on where GMSH cuts the surface,
  // so surfaces with holes (genus > 0) may need a different angle to succeed.
  auto attempt = [&](double classifyAngleDeg, bool lastAttempt) {
    gmsh::initialize(0, nullptr, false);
    // Failed intermediate attempts are reported by the retry loop instead.
    gmsh::option::setNumber("General.Terminal", lastAttempt ? 1 : 0);
    gmsh::model::add("gmsh_tetra_model");

    // ------------------------------------------------------------------ //
    // Build a discrete surface entity directly in memory (no temp file)   //
    // and populate it with the input triangle mesh.                       //
    // ------------------------------------------------------------------ //
    gmsh::model::addDiscreteEntity(2, 1);

    // Nodes (GMSH uses 1-based tags)
    std::vector<std::size_t> nodeTags;
    std::vector<double> nodeCoords;
    nodeTags.reserve(verts.size());
    nodeCoords.reserve(verts.size() * 3);
    for (std::size_t i = 0; i < verts.size(); ++i) {
      nodeTags.push_back(i + 1);
      nodeCoords.push_back(static_cast<double>(verts[i].x));
      nodeCoords.push_back(static_cast<double>(verts[i].y));
      nodeCoords.push_back(static_cast<double>(verts[i].z));
    }
    gmsh::model::mesh::addNodes(2, 1, nodeTags, nodeCoords);

    // Triangles – element type 2 = 3-node triangle
    std::vector<std::size_t> triNodeTags;
    triNodeTags.reserve(tris.size() * 3);
    for (const auto &t : tris) {
      triNodeTags.push_back(static_cast<std::size_t>(t.index[0]) + 1);
      triNodeTags.push_back(static_cast<std::size_t>(t.index[1]) + 1);
      triNodeTags.push_back(static_cast<std::size_t>(t.index[2]) + 1);
    }
    gmsh::model::mesh::addElementsByType(1, 2, {}, triNodeTags);

    // ------------------------------------------------------------------ //
    // Classify ("color") the surface and build a parametrized geometry.   //
    //                                                                      //
    // This follows GMSH's own tutorial for remeshing an STL file without   //
    // an underlying CAD model (see tutorials/c++/t13.cpp):                 //
    //   classifySurfaces -> createGeometry -> geo::addSurfaceLoop/addVolume//
    //                                                                      //
    // The angle parameter must be a moderate threshold (e.g. 40 degrees),  //
    // NOT 180 degrees. Using 180 keeps the whole closed surface as a       //
    // single patch, which cannot be parametrized (a closed genus-0 surface //
    // has no boundary curve to build a planar map from) and triggers       //
    // "Invalid exterior boundary mesh for parametrization".                //
    // ------------------------------------------------------------------ //
    const double classifyAngle = classifyAngleDeg * M_PI / 180.0;
    
    gmsh::model::mesh::classifySurfaces(classifyAngle, true, false);
    gmsh::model::mesh::createGeometry();

    // Collect all dim-2 entities produced by classification.
    gmsh::vectorpair surfs;
    gmsh::model::getEntities(surfs, 2);
    if (surfs.empty()) {
      throw std::runtime_error(
          "GMSHWrapper: classifySurfaces produced no surface entities. "
          "Check that the input mesh is a closed, manifold surface.");
    }
    std::vector<int> surfTags;
    surfTags.reserve(surfs.size());
    for (const auto &s : surfs)
      surfTags.push_back(s.second);

    // Build a volume from the parametrized surfaces via the geo kernel.
    const int surfaceLoopTag = gmsh::model::geo::addSurfaceLoop(surfTags);
    gmsh::model::geo::addVolume({surfaceLoopTag});
    gmsh::model::geo::synchronize();

    // ------------------------------------------------------------------ //
    // Mesh options and 3-D generation                                     //
    // ------------------------------------------------------------------ //
    gmsh::option::setNumber("Mesh.CharacteristicLengthMin", meshSizeMin);
    gmsh::option::setNumber("Mesh.CharacteristicLengthMax", meshSizeMax);
    gmsh::option::setNumber("Mesh.Algorithm3D",
                            static_cast<double>(algorithm3D));

    gmsh::model::mesh::generate(3);

    // ------------------------------------------------------------------ //
    // Extract nodes                                                       //
    // ------------------------------------------------------------------ //
    std::vector<std::size_t> outNodeTags;
    std::vector<double> outCoords;
    std::vector<double> outParamCoords;
    gmsh::model::mesh::getNodes(outNodeTags, outCoords, outParamCoords);

    std::map<std::size_t, int> nodeTagToIdx;
    tetraPoints.reserve(outNodeTags.size());
    for (std::size_t i = 0; i < outNodeTags.size(); ++i) {
      nodeTagToIdx[outNodeTags[i]] = static_cast<int>(i);
      Vec3f v;
      v.x = static_cast<float>(outCoords[i * 3]);
      v.y = static_cast<float>(outCoords[i * 3 + 1]);
      v.z = static_cast<float>(outCoords[i * 3 + 2]);
      tetraPoints.push_back(v);
    }

    // ------------------------------------------------------------------ //
    // Extract tetrahedral elements (GMSH element type 4 = 4-node tet)    //
    // ------------------------------------------------------------------ //
    std::vector<int> elemTypes;
    std::vector<std::vector<std::size_t>> elemTags;
    std::vector<std::vector<std::size_t>> elemNodeTagsOut;
    gmsh::model::mesh::getElements(elemTypes, elemTags, elemNodeTagsOut, 3, -1);

    for (std::size_t t = 0; t < elemTypes.size(); ++t) {
      if (elemTypes[t] == 4) { // 4-node linear tetrahedron
        const auto &eTags = elemNodeTagsOut[t];
        tetraIndices.reserve(eTags.size() / 4);
        for (std::size_t i = 0; i + 3 < eTags.size(); i += 4) {
          Tetrahedron tet;
          tet.index[0] = nodeTagToIdx[eTags[i]];
          tet.index[1] = nodeTagToIdx[eTags[i + 1]];
          tet.index[2] = nodeTagToIdx[eTags[i + 2]];
          tet.index[3] = nodeTagToIdx[eTags[i + 3]];
          tetraIndices.push_back(tet);
        }
        break; // only one 4-node tet type expected
      }
    }

    if (tetraPoints.empty() || tetraIndices.empty()) {
      throw std::runtime_error("GMSH did not generate any tetrahedra.");
    }
    std::cout << "GMSHWrapper: " << tetraPoints.size() << " vertices, "
              << tetraIndices.size() << " tetrahedra." << std::endl;

    gmsh::finalize();
  };

  const double anglesDeg[] = {40.0, 20.0, 60.0, 30.0, 90.0, 15.0, 120.0};
  std::string lastError;
  for (const double angle : anglesDeg) {
    const bool isLast = (angle == anglesDeg[std::size(anglesDeg) - 1]);
    try {
      tetraPoints.clear();
      tetraIndices.clear();
      attempt(angle, isLast);
      return;
    } catch (const std::exception &e) {
      lastError = e.what();
      std::cerr << "GMSHWrapper: attempt with classify angle " << angle
                << " failed – " << lastError << std::endl;
      if (gmsh::isInitialized())
        gmsh::finalize();
    } catch (...) {
      lastError = "GMSH failed with an unknown error.";
      std::cerr << "GMSHWrapper: attempt with classify angle " << angle
                << " failed - unknown error" << std::endl;
      if (gmsh::isInitialized())
        gmsh::finalize();
    }
  }
  throw std::runtime_error(lastError);
}

std::vector<Vec3f> &GMSHWrapper::GetTetraVertices() { return tetraPoints; }

std::vector<Tetrahedron> &GMSHWrapper::GetTetras() { return tetraIndices; }

