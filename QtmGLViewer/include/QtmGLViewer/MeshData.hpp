/*
 * MeshData.hpp
 *
 * CPU side geometry container plus generators for simple primitives
 * (sphere, arrows, grid, boxes) that can be uploaded into a MeshBuffer.
 */

#ifndef QTMGLVIEWER_MESHDATA_HPP_
#define QTMGLVIEWER_MESHDATA_HPP_

#include <QMatrix4x4>
#include <QVector3D>
#include <cstdint>
#include <vector>

namespace qtm {

struct MeshData {
  std::vector<QVector3D> positions;
  std::vector<QVector3D> normals;  // optional, one per position
  std::vector<QVector3D> colors;   // optional, RGB in [0, 1]
  std::vector<uint32_t> indices;   // optional

  void clear();
  // Appends another mesh, transforming its positions/normals by transform.
  void append(const MeshData &other,
              const QMatrix4x4 &transform = QMatrix4x4());
  // Sets the same color for every vertex.
  void setColor(const QVector3D &color);
};

namespace primitives {

// Moebius strip in the XY plane with the given center circle radius and
// strip half width. Indexed triangles with normals and vertex colors running
// through the full hue spectrum along the strip.
MeshData mobiusStrip(float radius, float halfWidth, int segments);
// Open truncated cone along +Z from z0 (radius r0) to z1 (radius r1),
// indexed triangles with normals.
MeshData truncatedCone(float r0, float r1, float z0, float z1, int slices);
// libQGLViewer style arrow along +Z (indexed triangles with normals).
MeshData arrow(float length, float radius, int slices = 12);
// Line grid in the XY plane from -size to +size (GL_LINES).
MeshData grid(float size, int subdivisions = 10);
// "X", "Y" and "Z" letters drawn with lines at the tips of axes of the
// given length (GL_LINES).
MeshData axisLetters(float length);
// Appends the 12 edges of an axis aligned box (GL_LINES).
void appendBoxLines(MeshData &mesh, const QVector3D &min,
                    const QVector3D &max);
// Converts an indexed triangle mesh into a unique edge list (GL_LINES).
std::vector<uint32_t> triangleEdges(const std::vector<uint32_t> &triangles);

}  // namespace primitives

}  // namespace qtm

#endif /* QTMGLVIEWER_MESHDATA_HPP_ */
