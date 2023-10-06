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
#include "tetrastuffingdialog.h"
#include "tetgendialog.h"
#include <QCloseEvent>
#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

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

private:
  Ui::MainWindow *ui;
  TetraStuffingDialog *tsd = NULL;
  CGALTetrahedralizeDialog *ctd = NULL;
  QuartetTetraStuffingDialog *cts = NULL;
  TetgenDialog *ttd = NULL;

protected:
  // override main window close event to close all child dialogs
  void closeEvent(QCloseEvent *event);
  // drag and drop of mesh files on the main window
  void dragEnterEvent(QDragEnterEvent *event_);
  void dropEvent(QDropEvent *event_);

private slots:
  void on_viewer_onLoad();
};

#endif // MAINWINDOW_H
