/*
 * SphereViewer.cpp
 */

#include "SphereViewer.hpp"

#include <QOpenGLExtraFunctions>
#include <QtmGLViewer/MeshData.hpp>

SphereViewer::SphereViewer(QWidget *parent) : QtmGLViewer(parent) {
  // the grid extends a bit beyond the sphere
  setSceneRadius(1.5f * radius_);
  showEntireScene();
}

SphereViewer::~SphereViewer() {
  makeCurrent();
  cleanupScene();
  doneCurrent();
}

void SphereViewer::setSphereRadius(double radius) {
  radius_ = float(radius);
  dirty_ = true;
  setSceneRadius(1.5f * radius_);
  showEntireScene();
}

void SphereViewer::setSubdivisions(int subdivisions) {
  subdivisions_ = subdivisions;
  dirty_ = true;
  update();
}

void SphereViewer::setWireframeDrawn(bool drawn) {
  wireframeDrawn_ = drawn;
  update();
}

void SphereViewer::setSphereColor(const QColor &color) {
  color_ = color;
  update();
}

void SphereViewer::initializeScene() { dirty_ = true; }

void SphereViewer::cleanupScene() {
  sphere_.destroy();
  edges_.destroy();
  dirty_ = true;
}

void SphereViewer::rebuildSphere() {
  qtm::MeshData mesh = qtm::primitives::icosphere(radius_, subdivisions_);
  vertexCount_ = int(mesh.positions.size());
  triangleCount_ = int(mesh.indices.size() / 3);
  sphere_.upload(mesh, GL_TRIANGLES);

  // edges share the vertex positions, only the indices differ
  mesh.normals.clear();
  mesh.indices = qtm::primitives::triangleEdges(mesh.indices);
  edges_.upload(mesh, GL_LINES);
  dirty_ = false;
}

void SphereViewer::draw(const qtm::RenderContext &context) {
  if (dirty_) rebuildSphere();

  QOpenGLExtraFunctions *gl = context.gl();
  gl->glEnable(GL_CULL_FACE);
  // push the faces back a little so the wireframe is not z-fighting
  gl->glEnable(GL_POLYGON_OFFSET_FILL);
  gl->glPolygonOffset(1.0f, 1.0f);
  context.drawSolid(sphere_, color_);
  gl->glDisable(GL_POLYGON_OFFSET_FILL);
  gl->glDisable(GL_CULL_FACE);
  if (wireframeDrawn_) context.drawLines(edges_, QColor(230, 230, 30));

  drawText(10, 45,
           tr("Sphere: radius %1, %2 vertices, %3 triangles")
               .arg(double(radius_))
               .arg(vertexCount_)
               .arg(triangleCount_),
           QColor(25, 204, 25));
  drawText(10, 60,
           tr("Scene radius: %1, %2 projection")
               .arg(double(sceneRadius()))
               .arg(isOrthographic() ? tr("orthographic")
                                     : tr("perspective")),
           QColor(204, 204, 25));
}
