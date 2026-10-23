/*
 * MeshData.cpp
 */

#include "QtmGLViewer/MeshData.hpp"

#include <QColor>
#include <QtMath>
#include <algorithm>
#include <cmath>
#include <set>
#include <utility>

namespace qtm {

void MeshData::clear() {
  positions.clear();
  normals.clear();
  colors.clear();
  indices.clear();
}

void MeshData::append(const MeshData &other, const QMatrix4x4 &transform) {
  const uint32_t offset = uint32_t(positions.size());
  const QMatrix3x3 normalMatrix = transform.normalMatrix();
  for (const QVector3D &p : other.positions)
    positions.push_back(transform.map(p));
  if (!other.normals.empty()) {
    normals.resize(offset, QVector3D(0.0f, 0.0f, 1.0f));
    for (const QVector3D &n : other.normals) {
      const float *m = normalMatrix.constData();  // column major
      normals.push_back(
          QVector3D(m[0] * n.x() + m[3] * n.y() + m[6] * n.z(),
                    m[1] * n.x() + m[4] * n.y() + m[7] * n.z(),
                    m[2] * n.x() + m[5] * n.y() + m[8] * n.z())
              .normalized());
    }
  }
  if (!other.colors.empty()) {
    colors.resize(offset, QVector3D(1.0f, 1.0f, 1.0f));
    colors.insert(colors.end(), other.colors.begin(), other.colors.end());
  }
  for (uint32_t i : other.indices) indices.push_back(offset + i);
}

void MeshData::setColor(const QVector3D &color) {
  colors.assign(positions.size(), color);
}

namespace primitives {

MeshData mobiusStrip(float radius, float halfWidth, int segments) {
  MeshData mesh;
  segments = std::max(segments, 3);
  for (int i = 0; i <= segments; ++i) {
    const float t = float(i) / float(segments);
    const float u = 2.0f * float(M_PI) * t;
    const float c = std::cos(u), s = std::sin(u);
    const float ch = std::cos(0.5f * u), sh = std::sin(0.5f * u);
    const QColor hue = QColor::fromHsvF(t * 0.999f, 1.0f, 1.0f);
    const QVector3D color(hue.redF(), hue.greenF(), hue.blueF());
    // the last ring coincides with the first one, but with the sides swapped
    for (float v : {-halfWidth, halfWidth}) {
      const float r = radius + v * ch;
      const QVector3D du(-r * s - 0.5f * v * sh * c, r * c - 0.5f * v * sh * s,
                         0.5f * v * ch);
      const QVector3D dv(ch * c, ch * s, sh);
      mesh.positions.push_back(QVector3D(r * c, r * s, v * sh));
      mesh.normals.push_back(QVector3D::crossProduct(du, dv).normalized());
      mesh.colors.push_back(color);
    }
  }
  for (int i = 0; i < segments; ++i) {
    const uint32_t a = uint32_t(2 * i), b = a + 1, c = a + 2, d = a + 3;
    mesh.indices.insert(mesh.indices.end(), {a, c, b, b, c, d});
  }
  return mesh;
}

MeshData truncatedCone(float r0, float r1, float z0, float z1, int slices) {
  MeshData mesh;
  slices = std::max(slices, 3);
  const float height = z1 - z0;
  for (int i = 0; i <= slices; ++i) {
    const float angle = 2.0f * float(M_PI) * float(i) / float(slices);
    const float c = std::cos(angle);
    const float s = std::sin(angle);
    const QVector3D normal = QVector3D(c * height, s * height, r0 - r1)
                                 .normalized();
    mesh.positions.push_back(QVector3D(r0 * c, r0 * s, z0));
    mesh.positions.push_back(QVector3D(r1 * c, r1 * s, z1));
    mesh.normals.push_back(normal);
    mesh.normals.push_back(normal);
  }
  for (int i = 0; i < slices; ++i) {
    const uint32_t b0 = uint32_t(2 * i), t0 = b0 + 1;
    const uint32_t b1 = b0 + 2, t1 = b0 + 3;
    mesh.indices.insert(mesh.indices.end(), {b0, b1, t1, b0, t1, t0});
  }
  return mesh;
}

MeshData arrow(float length, float radius, int slices) {
  // same proportions as QGLViewer::drawArrow()
  if (radius < 0.0f) radius = 0.05f * length;
  const float head = 2.5f * (radius / length) + 0.1f;
  const float coneRadiusCoef = 4.0f - 5.0f * head;
  MeshData mesh = truncatedCone(radius, radius, 0.0f,
                                length * (1.0f - head / coneRadiusCoef),
                                slices);
  mesh.append(truncatedCone(coneRadiusCoef * radius, 0.0f,
                            length * (1.0f - head), length, slices));
  return mesh;
}

MeshData grid(float size, int subdivisions) {
  MeshData mesh;
  subdivisions = std::max(subdivisions, 1);
  for (int i = 0; i <= subdivisions; ++i) {
    const float pos = size * (2.0f * float(i) / float(subdivisions) - 1.0f);
    mesh.positions.push_back(QVector3D(pos, -size, 0.0f));
    mesh.positions.push_back(QVector3D(pos, size, 0.0f));
    mesh.positions.push_back(QVector3D(-size, pos, 0.0f));
    mesh.positions.push_back(QVector3D(size, pos, 0.0f));
  }
  return mesh;
}

MeshData axisLetters(float length) {
  // same strokes as QGLViewer::drawAxis()
  const float w = length / 40.0f;
  const float h = length / 30.0f;
  const float s = 1.04f * length;
  MeshData mesh;
  mesh.positions = {
      // X
      {s, w, -h}, {s, -w, h}, {s, -w, -h}, {s, w, h},
      // Y
      {w, s, h}, {0, s, 0}, {-w, s, h}, {0, s, 0}, {0, s, 0}, {0, s, -h},
      // Z
      {-w, h, s}, {w, h, s}, {w, h, s}, {-w, -h, s}, {-w, -h, s}, {w, -h, s}};
  return mesh;
}

void appendBoxLines(MeshData &mesh, const QVector3D &min,
                    const QVector3D &max) {
  const QVector3D c[8] = {
      {min.x(), min.y(), min.z()}, {max.x(), min.y(), min.z()},
      {max.x(), max.y(), min.z()}, {min.x(), max.y(), min.z()},
      {min.x(), min.y(), max.z()}, {max.x(), min.y(), max.z()},
      {max.x(), max.y(), max.z()}, {min.x(), max.y(), max.z()}};
  static const int edges[24] = {0, 1, 1, 2, 2, 3, 3, 0, 4, 5, 5, 6,
                                6, 7, 7, 4, 0, 4, 1, 5, 2, 6, 3, 7};
  if (mesh.indices.empty()) {
    for (int e : edges) mesh.positions.push_back(c[e]);
  } else {
    const uint32_t offset = uint32_t(mesh.positions.size());
    mesh.positions.insert(mesh.positions.end(), c, c + 8);
    for (int e : edges) mesh.indices.push_back(offset + uint32_t(e));
  }
}

std::vector<uint32_t> triangleEdges(const std::vector<uint32_t> &triangles) {
  std::set<std::pair<uint32_t, uint32_t>> edges;
  for (size_t i = 0; i + 2 < triangles.size(); i += 3) {
    for (int k = 0; k < 3; ++k) {
      const uint32_t a = triangles[i + k];
      const uint32_t b = triangles[i + (k + 1) % 3];
      edges.insert(std::make_pair(std::min(a, b), std::max(a, b)));
    }
  }
  std::vector<uint32_t> result;
  result.reserve(edges.size() * 2);
  for (const auto &e : edges) {
    result.push_back(e.first);
    result.push_back(e.second);
  }
  return result;
}

}  // namespace primitives

}  // namespace qtm
