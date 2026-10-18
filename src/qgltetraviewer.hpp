/*
 * qgltetraviewer.h
 *
 *  Created on: Sep 10, 2011
 *      Author: Dennis Luebke
 */

#ifndef Q_TETRA_VIEWER_H_
#define Q_TETRA_VIEWER_H_

#include <QElapsedTimer>
#include <QTimer>
#include <QtmGLViewer/QtmGLViewer.hpp>
#include <map>

#include "QGLTetraMesh.hpp"

class QGLTetraViewer : public QtmGLViewer {
  Q_OBJECT

 public:
  QGLTetraViewer(QWidget *parent);
  ~QGLTetraViewer() override;

  QGLTetraMesh *tMesh;

  void generateOctree(int d);

  /// Shows a "work in progress" indicator with the given text (and an
  /// optional smaller hint line) in the upper right corner until endTask()
  /// is called with the same id.
  void beginTask(const QString &id, const QString &text,
                 const QString &hint = QString());
  void endTask(const QString &id);
  /// Shows a short notice (with an optional smaller detail line) at the top
  /// of the viewer for the given time.
  void showNotice(const QString &text, const QString &detail = QString(),
                  int durationMs = 5000);
  /// repair defects of surface meshes when loading them (default: on)
  void setRepairOnLoad(bool repair_) { repairOnLoad = repair_; }
  float getMaxBBox() const;

 public slots:
  void ToggleTetraVis(bool drawWireframe_, bool drawSolid_);
  void ToggleTriangleVis(int i);
  void ToggleOctreeVis(int i);
  void ToggleBBox(int i);
  void ShowStatusMessage(const QString &msg_, int duration_ = 2000);
  bool loadSurface();
  bool loadGMSH();
  bool loadTetgen();
  void saveGMSH();
  void saveTetgen();
  void saveSurface();
  void selectSurfaceColor();
  void selectSurfaceColorWireframe();
  void selectTetraColor();
  void selectTetraOuterColor();
  void selectTetraInnerColor();
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
  void drawOverlay(QPainter &painter) override;
  QString helpString() const;
  QString helpHtml() const;

  qtm::MeshBuffer spiral;  // shown while no mesh is loaded

  struct RunningTask {
    QString text;
    QString hint;
    QElapsedTimer timer;
  };
  std::map<QString, RunningTask> runningTasks;
  QTimer taskAnimationTimer;  // repaints the indicator while tasks run

  void drawTaskOverlay(QPainter &painter);
  void drawNoticeOverlay(QPainter &painter);
  QString noticeText;
  QString noticeDetail;
  QTimer noticeTimer;  // hides the notice

  bool repairOnLoad = true;

  void updateSceneFromMesh();  // sets _maxBBox, scene center and radius

  float _maxBBox = 1.0f;  // maximum value in BBox lengths to determine suggested tetra
                   // size
};

#endif /* Q_TETRA_VIEWER_H_ */
