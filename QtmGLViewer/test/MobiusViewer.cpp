/*
 * MobiusViewer.cpp
 */

#include "MobiusViewer.hpp"

#include <QOpenGLExtraFunctions>
#include <QtmGLViewer/MeshData.hpp>

MobiusViewer::MobiusViewer(QWidget *parent) : QtmGLViewer(parent) {
  // the grid extends a bit beyond the strip
  setSceneRadius(2.0f * radius_);
  showEntireScene();
}

MobiusViewer::~MobiusViewer() {
  makeCurrent();
  cleanupScene();
  doneCurrent();
}

void MobiusViewer::setStripRadius(double radius) {
  radius_ = float(radius);
  dirty_ = true;
  setSceneRadius(2.0f * radius_);
  showEntireScene();
}

void MobiusViewer::setWireframeDrawn(bool drawn) {
  wireframeDrawn_ = drawn;
  update();
}

void MobiusViewer::initializeScene() { dirty_ = true; }

void MobiusViewer::cleanupScene() {
  strip_.destroy();
  edges_.destroy();
  dirty_ = true;
}

void MobiusViewer::rebuildStrip() {
  // tilted so the strip is not seen edge-on or from straight above
  QMatrix4x4 tilt;
  tilt.rotate(-45.0f, 1.0f, 0.0f, 0.0f);
  tilt.rotate(35.0f, 0.0f, 1.0f, 0.0f);
  qtm::MeshData mesh;
  const float halfWidth = 0.4375f * radius_;
  mesh.append(qtm::primitives::mobiusStrip(radius_, -halfWidth, halfWidth, 400),
              tilt);
  vertexCount_ = int(mesh.positions.size());
  triangleCount_ = int(mesh.indices.size() / 3);
  strip_.upload(mesh, GL_TRIANGLES);

  // edges share the vertex positions, only the indices differ
  mesh.normals.clear();
  mesh.colors.clear();
  mesh.indices = qtm::primitives::triangleEdges(mesh.indices);
  edges_.upload(mesh, GL_LINES);
  dirty_ = false;
}

void MobiusViewer::draw(const qtm::RenderContext &context) {
  if (dirty_) rebuildStrip();

  QOpenGLExtraFunctions *gl = context.gl();
  // push the faces back a little so the wireframe is not z-fighting
  gl->glEnable(GL_POLYGON_OFFSET_FILL);
  gl->glPolygonOffset(1.0f, 1.0f);
  context.drawSolid(strip_, QColor());
  gl->glDisable(GL_POLYGON_OFFSET_FILL);
  if (wireframeDrawn_) context.drawLines(edges_, QColor(230, 230, 30));

  drawText(10, 45,
           tr("Moebius strip: radius %1, %2 vertices, %3 triangles")
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
