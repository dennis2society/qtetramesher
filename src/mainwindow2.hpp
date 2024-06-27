/*
 * mainwindow.h
 *
 *  Created 2011
 *      Author: Dennis Luebke
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <CGALTetrahedralizeWidget.hpp>
#include <OctreeVisWidget.hpp>
#include <QAction>
#include <QCloseEvent>
#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QLayout>
#include <QMainWindow>
#include <QMenuBar>
#include <QPushButton>
#include <QWidget>
#include <SofaTetraStuffingWidget.hpp>
#include <SurfaceVisWidget.hpp>
#include <TetraVisWidget.hpp>

#include "cgaltetrahedralizedialog.h"
#include "qgltetraviewer.h"
#include "quartetterahedralizedialog.h"
#include "tetgendialog.h"
#include "tetrastuffingdialog.h"

// namespace Ui {
// class MainWindow;
// }

class QTetraMesherMainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit QTetraMesherMainWindow(QWidget *parent = 0);
  //~QTetraMesherMainWindow();

 public slots:
  void toggleFullScreen(bool value);
  void showTetraStuffingDialog();
  void showQuartetTetraDialog();
  void showCGALDialog();
  void showTetgenDialog();
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
  void tetraVisChangedSlot();
  void tetraColorButtonSlot();
  void tetraWirefraceColorButtonSlot();
  void cutplaneSliderSlot();
  void loadSurfaceSlot();
  void tetraMethodComboBoxSlot();
  void generateSofaTetraStuffingSlot();
  void generateCGALTetrahedralizeSlot();

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
  QVBoxLayout optionsLayout;
  QMenu fileMenu;
  QMenu helpMenu;
  QMenu viewMenu;
  // Menu bar actions
  QAction actionLoadSurface;
  QAction actionLoadGMSH;
  QAction actionExportGMSH;
  QAction actionExportSurface;
  QAction actionQuit;
  QAction actionShowAxis;
  QAction actionShowGrid;
  QAction actionShowBBox;
  QAction actionAbout;
  QAction actionHelp;
  // Visualization options and task widgets
  SurfaceVisWidget surfaceVisWidget;
  TetraVisWidget tetraVisWidget;
  OctreeVisWidget octreeVisWidget;
  QLabel tetraMeshMethodLabel;
  QComboBox tetraMeshMethodComboBox;
  SofaTetraStuffingWidget sofaTetraStuffingWidget;
  CGALTetrahedralizeWidget cgalTetrahedralizeWidget;
  // Ui::MainWindow *ui;
  TetraStuffingDialog *tsd = NULL;
  CGALTetrahedralizeDialog *ctd = NULL;
  QuartetTetraStuffingDialog *cts = NULL;
  TetgenDialog *ttd = NULL;
  uint selectedTetraMethod;

  void setupUI();
  void connectSlots();

 protected:
  // override main window close event to close all child dialogs
  void closeEvent(QCloseEvent *event);

  void resizeEvent(QResizeEvent *event);

 private slots:
  void on_viewer_onLoad();
};

#endif  // MAINWINDOW_H
