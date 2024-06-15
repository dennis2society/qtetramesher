/*
 * mainwindow.h
 *
 *  Created 2011
 *      Author: Dennis Luebke
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "cgaltetrahedralizedialog.h"
#include "quartetterahedralizedialog.h"
#include "tetgendialog.h"
#include "tetrastuffingdialog.h"
#include <QCloseEvent>
#include <QMainWindow>
#include <QMenuBar>
#include <QAction>
#include <QLayout>
#include <QFrame>
#include "qgltetraviewer.h"
#include <QWidget>
#include <QPushButton>
#include <QGridLayout>

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
  void generateOctree();
  void displayMessage(QString msg_, unsigned int time_);
  void notifyDone(QString msg_, unsigned int time_);
  void clearTetraOptions(); // call this when loading a new mesh to reset the
                            // settings
  void buttonSlot();

private:
  // QGLViewer variant
  QGLTetraViewer *viewer;
  // UI elements
  QWidget centralWidget;
  QGridLayout centralLayout;
  QHBoxLayout mainLayout;
  QFrame viewerFrame;
  QFrame optionsFrame;
  QVBoxLayout viewerLayout;
  QVBoxLayout optionsLayout;
  QMenu fileMenu;
  QMenu helpMenu;
  QMenu meshMenu;
  // Menu bar actions
  QAction actionLoadSurface;
  QAction actionLoadGMSH;
  QAction actionExportGMSH;
  QAction actionExportSurface;
  QAction actionQuit;
  QAction actionAbout;
  QAction actionHelp;
  QPushButton huiButton;
  // Ui::MainWindow *ui;
  TetraStuffingDialog *tsd = NULL;
  CGALTetrahedralizeDialog *ctd = NULL;
  QuartetTetraStuffingDialog *cts = NULL;
  TetgenDialog *ttd = NULL;

protected:
  // override main window close event to close all child dialogs
  void closeEvent(QCloseEvent *event);

private slots:
  void on_viewer_onLoad();
};

#endif // MAINWINDOW_H
