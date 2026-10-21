/*
 * mainwindow.h
 *
 *  Created 2011
 *      Author: Dennis Luebke
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <CGALTetrahedralizeWidget.hpp>
#include <GMSHWidget.hpp>
#include <FTetWildWidget.hpp>
#include <OctreeVisWidget.hpp>
#include <QAction>
#include <QCheckBox>
#include <QCloseEvent>
#include <QFrame>
#include <QFutureWatcher>
#include <atomic>
#include <QGridLayout>
#include <QLabel>
#include <QLayout>
#include <QMainWindow>
#include <QMenuBar>
#include <QPushButton>
#include <QScrollArea>
#include <QWidget>
#include <GenericIsosurfaceStuffingWidget.hpp>
#include <SofaTetraStuffingWidget.hpp>
#include <SurfaceVisWidget.hpp>
#include <TetgenWidget.hpp>
#include <TetraVisWidget.hpp>

#include "TetraJob.hpp"
#include "TetraMeshTools/SurfaceChecker.h"
#include "qgltetraviewer.hpp"

class QTetraMesherMainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit QTetraMesherMainWindow(QWidget *parent = 0);
  ~QTetraMesherMainWindow() override;

 public slots:
  void toggleFullScreen(bool value);
  void generateOctreeSlot();
  void octreeVisChangedSlot();
  void displayMessage(QString msg_, unsigned int time_);
  void notifyDone(QString msg_, unsigned int time_);
  void setAxisShownSlot();
  void setGridShownSlot();
  void setBBoxVisibleSlot();
  void surfaceVisChanged();
  void surfaceColorButtonSlot();
  void surfaceWireframeColorButtonSlot();
  void checkSurfaceSlot();
  void tetraVisChangedSlot();
  void tetraColorButtonSlot();
  void tetraOuterColorButtonSlot();
  void tetraInnerColorButtonSlot();
  void depthColoringSlot(bool enabled_);
  void tetraWirefraceColorButtonSlot();
  void cutplaneXSliderSlot();
  void cutplaneYSliderSlot();
  void cutplaneZSliderSlot();
  void loadSurfaceSlot();
  void exportSurfaceSlot();
  void loadGMSHSlot();
  void exportGMSHSlot();
  void loadTetgenSlot();
  void exportTetgenSlot();
  void tetraMethodComboBoxSlot();
  void generateSofaTetraStuffingSlot();
  void generateCGALTetrahedralizeSlot();
  void generateGenericIsosurfaceStuffingSlot();
  void generateTetgenSlot();
  void generateGMSHSlot();
  void generateFTetWildSlot();

 private:
  // QGLViewer variant
  QGLTetraViewer *viewer;
  // UI elements
  QWidget centralWidget;
  QGridLayout centralLayout;
  QHBoxLayout mainLayout;
  QFrame viewerFrame;
  QFrame surfaceOptionsFrame;
  QVBoxLayout viewerLayout;
  // declared before optionsLayout so that the layout is destroyed first
  QScrollArea optionsScrollArea;
  QWidget optionsContainer;
  QVBoxLayout optionsLayout;
  QMenu fileMenu;
  QMenu helpMenu;
  QMenu viewMenu;
  // Menu bar actions
  QAction actionLoadSurface;
  QAction actionLoadGMSH;
  QAction actionExportGMSH;
  QAction actionLoadTetgen;
  QAction actionExportTetgen;
  QAction actionExportSurface;
  QAction actionQuit;
  QAction actionShowAxis;
  QAction actionShowGrid;
  QAction actionShowBBox;
  QAction actionAbout;
  QAction actionHelp;
  // Visualization options and task widgets
  QCheckBox repairOnLoadCheckBox;
  SurfaceVisWidget surfaceVisWidget;
  TetraVisWidget tetraVisWidget;
  OctreeVisWidget octreeVisWidget;
  // Tetra mesh methods widgets
  QLabel tetraMeshMethodLabel;
  QComboBox tetraMeshMethodComboBox;
  SofaTetraStuffingWidget sofaTetraStuffingWidget;
  CGALTetrahedralizeWidget cgalTetrahedralizeWidget;
  GenericIsosurfaceStuffingWidget genericIsosurfaceStuffingWidget;
  TetgenWidget tetgenWidget;
  GMSHWidget gmshWidget;
  FTetWildWidget ftetwildWidget;

  void setupUI();
  void connectSlots();

  void updateCutplaneSliders();
  /// sets the default sizes of the methods from the mesh bounding box
  void suggestMethodParameters();
  /// shows the name of the loaded file in the title bar
  void updateWindowTitle();
  void gmshLoadFinished(std::unique_ptr<TetraJob::Result> r,
                        const QString &fileName);

  // Background tasks: tetrahedralization (worker process, can be
  // canceled), octree generation and surface check (threads). While any of
  // them runs, loading another mesh is disabled because the tasks read the
  // current surface mesh.
  void startOrCancelTetraJob(TetraMethodWidget &widget);
  void updateBusyState();
  void stopBackgroundTasks();
  void showSurfaceCheckResult(const SurfaceCheckResult &r);
  std::vector<TetraMethodWidget *> tetraMethodWidgets();

  TetraJob tetraJob;
  TetraMethodWidget *runningTetraWidget = nullptr;
  QString loadingGMSHFile;  // file loaded by the running tetraJob
  QFutureWatcher<Octree *> octreeWatcher;
  const void *octreeSurface = nullptr;  // surface the octree is built from
  bool octreePending = false;
  QFutureWatcher<std::shared_ptr<SurfaceCheckResult>> surfaceCheckWatcher;
  // set when the window closes, stops the octree and surface check threads
  std::atomic<bool> stopThreads{false};
  void updateColorButtons();
  void loadSettings();
  void saveSettings();

 protected:
  // override main window close event to close all child dialogs
  void closeEvent(QCloseEvent *event);

 private slots:
  void tetraJobFinished();
  void octreeFinished();
  void surfaceCheckFinished();
  void on_viewer_onLoad();
};

#endif  // MAINWINDOW_H
