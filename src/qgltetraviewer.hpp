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
#include <deque>
#include <map>

#include "QGLTetraMesh.hpp"
#include "TetraMeshTools/TetraMeshQuality.h"

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
  /// Console output of a running job, shown in the lower right corner.
  /// beginJobOutput() clears it, endJobOutput() hides it after hideDelayMs.
  void beginJobOutput();
  void appendJobOutput(const QString &line, bool isError, bool replaceLast);
  void endJobOutput(int hideDelayMs = 5000);
  void setJobOutputEnabled(bool enabled_);
  /// repair defects of surface meshes when loading them (default: on)
  void setRepairOnLoad(bool repair_) { repairOnLoad = repair_; }
  float getMaxBBox() const;
  /// file of the currently shown mesh (empty if none was loaded)
  const QString &loadedFileName() const { return currentFileName; }
  /// Asks for a GMSH file to load, returns an empty string on cancel. The
  /// file is loaded by the main window with a TetraJob.
  QString selectGMSHFile();
  /// Call after a mesh was loaded from fileName_ into tMesh: fits the view
  /// to the new mesh and shows message_ in the status line.
  void showLoadedMesh(const QString &fileName_, const QString &message_);
  /// Loads a surface mesh without asking for the file (e.g. a recent file).
  bool loadSurfaceFile(const QString &fileName_);
  /// duration of the last successful Tetgen load
  qint64 lastLoadMs() const { return lastLoadMs_; }
  /// Shows the measurements in the upper right corner while the current
  /// tetra mesh is shown; elapsedMs_ < 0 hides the time.
  void setMeshQuality(const TetraTools::TetraMeshQuality &q, const QString &source,
                      qint64 elapsedMs_);
  void setMeshQualityEnabled(bool enabled_);
  /// Scales/rotates the loaded surface mesh (see
  /// QGLTetraMesh::SetSurfaceTransform()). fitView_ refits the camera.
  bool transformSurface(float scale_, float xDeg_, float yDeg_, float zDeg_,
                        bool fitView_);
  /// Fast GPU-only preview while a slider is dragged, see
  /// QGLTetraMesh::PreviewSurfaceTransform().
  bool previewSurfaceTransform(float scale_, float xDeg_, float yDeg_,
                               float zDeg_);
  /// see QGLTetraMesh::ReplaceSurface()
  bool replaceSurface(const std::vector<Vec3f> &verts_,
                      const std::vector<Triangle> &tris_);

 public slots:
  void ToggleTetraVis(bool drawWireframe_, bool drawSolid_);
  void ToggleTriangleVis(int i);
  void ToggleOctreeVis(int i);
  void ToggleBBox(int i);
  void ShowStatusMessage(const QString &msg_, int duration_ = 2000);
  bool loadSurface();
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

  qtm::MeshBuffer mobius;  // shown while no mesh is loaded
  qtm::MeshBuffer mobiusOutline;

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

  struct OutputLine {
    QString text;
    bool isError;
  };
  void drawJobOutputOverlay(QPainter &painter);
  std::deque<OutputLine> jobOutput;
  bool jobOutputEnabled = true;
  bool jobOutputVisible = false;
  QTimer jobOutputTimer;  // hides the output after the job

  bool repairOnLoad = true;
  QString currentFileName;
  qint64 lastLoadMs_ = -1;

  void drawQualityOverlay(QPainter &painter);
  TetraTools::TetraMeshQuality meshQuality;
  QString meshQualitySource;
  qint64 meshQualityElapsedMs = -1;
  const void *meshQualityMesh = nullptr;  // tetra mesh the values belong to
  bool meshQualityEnabled = true;

  void updateSceneFromMesh();  // sets _maxBBox, scene center and radius

  float _maxBBox = 1.0f;  // maximum value in BBox lengths to determine suggested tetra
                   // size
};

#endif /* Q_TETRA_VIEWER_H_ */
