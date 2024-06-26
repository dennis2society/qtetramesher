/*
 * mainwindow.cpp
 *
 *  Created 2012
 *      Author: Dennis Luebke
 */

#include "mainwindow.h"
#include "QMimeData"
#include "ui_mainwindow.h"
#include <QGLViewer/manipulatedFrame.h>
#include <QUrl>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  ui->viewer->setFPSIsDisplayed(true);
  ui->viewer->setAxisIsDrawn(true);
  ui->viewer->setGridIsDrawn(true);
  this->setAcceptDrops(true);
  ui->viewer->setAcceptDrops(true);
  setlocale(LC_NUMERIC, "C");
  tsd = NULL;
  ctd = NULL;
  ui->viewer->setManipulatedFrame(new qglviewer::ManipulatedFrame);
  ui->viewer->manipulatedFrame()->setSpinningSensitivity(85.0);
  ui->viewer->manipulatedFrame()->setRotationSensitivity(0.2);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::toggleFullScreen(bool value) {
  if (value) {
    this->showFullScreen();
  } else {
    this->showNormal();
  }
}

void MainWindow::showTetraStuffingDialog() {
  if (tsd == NULL) {
    tsd = new TetraStuffingDialog();
    tsd->setViewer(ui->viewer);
    connect(tsd, SIGNAL(displayMessage(QString,uint)), this,
            SLOT(displayMessage(QString,uint)));
    connect(tsd, SIGNAL(notifyDone(QString,uint)), this,
            SLOT(notifyDone(QString,uint)));
    tsd->setTetraSize(ui->viewer->getMaxBBox() * 0.05f);
  }
  tsd->show();
}

void MainWindow::showQuartetTetraDialog() {
  if (cts == NULL) {
    cts = new QuartetTetraStuffingDialog();
    cts->setViewer(ui->viewer);
    connect(cts, SIGNAL(displayMessage(QString,uint)), this,
            SLOT(displayMessage(QString,uint)));
    connect(cts, SIGNAL(notifyDone(QString,uint)), this,
            SLOT(notifyDone(QString,uint)));
  }
  cts->show();
}

void MainWindow::showCGALDialog() {
  if (ctd == NULL) {
    ctd = new CGALTetrahedralizeDialog();
    ctd->setViewer(ui->viewer);
    connect(ctd, SIGNAL(displayMessage(QString,uint)), this,
            SLOT(displayMessage(QString,uint)));
    connect(ctd, SIGNAL(notifyDone(QString,uint)), this,
            SLOT(notifyDone(QString,uint)));
    ctd->setParams(ui->viewer->getMaxBBox() * 0.1f,
                   ui->viewer->getMaxBBox() * 0.05);
  }
  ctd->show();
}

void MainWindow::showTetgenDialog()
{
  //  if (ttd == NULL) {
  //    ttd = new TetgenDialog();
  //    ttd->setViewer(ui->viewer);
  //    connect(ttd, SIGNAL(displayMessage(QString,uint)), this,
  //            SLOT(displayMessage(QString,uint)));
  //    connect(ttd, SIGNAL(notifyDone(QString,uint)), this,
  //            SLOT(notifyDone(QString,uint)));
  //  }
  //  ttd->show();
  QMessageBox msg;
  msg.setWindowTitle("Tetgen disabled");
  msg.setText("Sorry, Tetgen tetrahedralize is disabled for now :(");
  msg.exec();
}

void MainWindow::generateOctree() {
  ui->viewer->generateOctree(ui->octreeDepthSpinbox->value());
  // ui->viewer->updateGL();
}

void MainWindow::displayMessage(QString msg_, unsigned int time_) {
  ui->viewer->ShowStatusMessage(msg_, time_);
}

void MainWindow::notifyDone(QString msg_, unsigned int time_) {
  ui->viewer->ShowStatusMessage(msg_, time_);
  // ui->viewer->updateGL();
}

void MainWindow::clearTetraOptions() {
  if (tsd != NULL) {
    tsd->hide();
    delete tsd;
    tsd = NULL;
  }
  if (ctd != NULL) {
    ctd->hide();
    delete ctd;
    ctd = NULL;
  }
  if (cts != NULL) {
    cts->hide();
    delete cts;
    cts = NULL;
  }
  if (ttd != NULL) {
    ttd->hide();
    delete ttd;
    ttd = NULL;
  }
}

void MainWindow::closeEvent(QCloseEvent *event) { clearTetraOptions(); }

void MainWindow::on_viewer_onLoad() {}
