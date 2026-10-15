/*
 * TriMeshLoader.h
 *
 *  Created on: Jan 04, 2013
 *      Author: Dennis Luebke
 *
 * Loads triangle surface meshes (OBJ, STL, PLY, Collada, 3DS, ...) using
 * the assimp library (https://github.com/assimp/assimp). All meshes of a
 * file are combined into one surface, which is optionally repaired (see
 * MeshRepair.h).
 *
 */

#ifndef TRIMESHLOADER_H_
#define TRIMESHLOADER_H_

#include "TetraMeshTools/GeometryTypes.h"
#include "TetraMeshTools/MeshRepair.h"
#include "TetraMeshTools/TetraToolsExports.h"
#include <string>
#include <vector>

namespace TetraTools {

class DLL_EXPORT TriMeshLoader {
public:
  TriMeshLoader();

  ~TriMeshLoader();

  const std::vector<Vec3f> &GetVertices() { return _vertices; }

  const std::vector<Triangle> &GetTriangles() { return _triangles; }

  /// Loads the file. With repair set, split vertices are merged and other
  /// defects fixed where possible (see RepairSurfaceMesh()).
  bool loadFile(const std::string &path_and_filename, bool repair = true);

  /// what the repair of the last loadFile() did
  const MeshRepairReport &GetRepairReport() const { return _repairReport; }

protected:
  std::vector<Vec3f> _vertices;
  std::vector<Triangle> _triangles;
  MeshRepairReport _repairReport;
};

} // namespace TetraTools

#endif /* TRIMESHLOADER_H_ */
