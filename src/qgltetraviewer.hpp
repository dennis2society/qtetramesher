/*
 * qgltetraviewer.h
 *
 *  Created on: Sep 10, 2011
 *      Author: Dennis Luebke
 */

#ifndef Q_TETRA_VIEWER_H_
#define Q_TETRA_VIEWER_H_

#include <QtmGLViewer/QtmGLViewer.hpp>

#include "QGLTetraMesh.hpp"

class QGLTetraViewer : public QtmGLViewer {
  Q_OBJECT

 public:
  QGLTetraViewer(QWidget *parent);
  ~QGLTetraViewer() override;

  QGLTetraMesh *tMesh;

  void generateOctree(int d);
  float getMaxBBox() const;

 public slots:
  void ToggleTetraVis(bool drawWireframe_, bool drawSolid_);
  void ToggleTriangleVis(int i);
  void ToggleOctreeVis(int i);
  void ToggleBBox(int i);
  void ShowStatusMessage(const QString &msg_, int duration_ = 2000);
  void loadSurface();
  void loadGMSH();
  void loadTetgen();
  void saveGMSH();
  void saveTetgen();
  void saveSurface();
  void selectSurfaceColor();
  void selectSurfaceColorWireframe();
  void selectTetraColor();
  void selectTetraColorWireframe();
  void scale10();
  void scale01();
  void showAbout();
  void showHelp();

 signals:
  void onLoad();

 protected:
  void initializeScene() override;
  void cleanupScene() override;
  void draw(const qtm::RenderContext &context) override;
  QString helpString() const;
  QString helpHtml() const;

  qtm::MeshBuffer spiral;  // shown while no mesh is loaded

  float _maxBBox;  // maximum value in BBox lengths to determine suggested tetra
                   // size
};

#endif /* Q_TETRA_VIEWER_H_ */
