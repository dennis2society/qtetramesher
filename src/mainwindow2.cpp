/*
 * mainwindow.cpp
 *
 *  Created 2012
 *      Author: Dennis Luebke
 */

#include "mainwindow2.hpp"
#include "QMimeData"
// #include "ui_mainwindow.h"
#include <QGLViewer/manipulatedFrame.h>
#include <QMessageBox>
#include <QUrl>
#include <iostream>

QTetraMesherMainWindow::QTetraMesherMainWindow(QWidget *parent)
    : QMainWindow(parent), surfaceVisWidget(this), tetraVisWidget(this), octreeVisWidget(this),
    sofaTetraStuffingWidget(this)
{
  //setlocale(LC_NUMERIC, "C");
  setupUI();
  connectSlots();
}

void QTetraMesherMainWindow::setupUI()
{
  QFont boldFont;
  boldFont.setBold(true);
  this->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
  // Menu stuff
  fileMenu.setTitle("File");
  this->menuBar()->addMenu(&fileMenu);
  actionLoadSurface.setText("Load Surface Mesh (obj, stl, dae, 3ds, off)");
  fileMenu.addAction(&actionLoadSurface);
  actionLoadGMSH.setText("Load GMSH");
  fileMenu.addAction(&actionLoadGMSH);
  fileMenu.addSeparator();
  actionExportGMSH.setText("Export GMSH");
  fileMenu.addAction(&actionExportGMSH);
  actionExportSurface.setText("Export Surface");
  fileMenu.addAction(&actionExportSurface);
  fileMenu.addSeparator();
  actionQuit.setText("Quit");
  fileMenu.addAction(&actionQuit);
  viewMenu.setTitle("View");
  menuBar()->addMenu(&viewMenu);
  actionShowBBox.setText("Show BBox");
  actionShowBBox.setCheckable(true);
  viewMenu.addAction(&actionShowBBox);

  helpMenu.setTitle("Help");
  menuBar()->addMenu(&helpMenu);
  actionAbout.setText("About");
  actionHelp.setText("Help");
  helpMenu.addAction(&actionAbout);
  helpMenu.addAction(&actionHelp);
  // central UI
  this->setCentralWidget(&centralWidget);
  centralWidget.setLayout(&centralLayout);
  centralWidget.setMinimumWidth(1024);
  centralWidget.setMinimumHeight(800);
  centralWidget.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
  viewerFrame.setMaximumSize(820, 760);
  viewerFrame.setMaximumSize(3840, 2160);
  viewerFrame.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
  viewerFrame.setFrameShape(QFrame::Box);
  viewerFrame.setFrameShadow(QFrame::Raised);
  viewer = new QGLTetraViewer(&viewerFrame);
  viewer->setFPSIsDisplayed(true);
  viewer->setAxisIsDrawn(true);
  viewer->setGridIsDrawn(true);
  viewer->setManipulatedFrame(new qglviewer::ManipulatedFrame);
  viewer->manipulatedFrame()->setSpinningSensitivity(85.0);
  viewer->manipulatedFrame()->setRotationSensitivity(0.2);
  viewer->setMinimumSize(820, 780);
  viewer->setMaximumSize(3840, 2160);
  viewer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
  viewerLayout.addWidget(&viewerFrame);
  //surfaceOptionsFrame.setMinimumSize(530, 220);
  //surfaceOptionsFrame.setMaximumSize(580, 260);
  surfaceOptionsFrame.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
  surfaceOptionsFrame.setFrameShape(QFrame::Box);
  surfaceOptionsFrame.setFrameShadow(QFrame::Raised);
  tetraMeshMethodLabel.setText("Tetra Mesh Method");
  tetraMeshMethodLabel.setFont(boldFont);
  tetraMeshMethodComboBox.addItem("SOFA TetraStuffing");
  tetraMeshMethodComboBox.addItem("CGAL Tetrahedralize");
  tetraMeshMethodComboBox.addItem("Quartet TetraStuffing");
  tetraMeshMethodComboBox.addItem("Tetgen TetraStuffing");
  optionsLayout.addWidget(&surfaceVisWidget);
  optionsLayout.addWidget(&tetraVisWidget);
  optionsLayout.addWidget(&octreeVisWidget);
  optionsLayout.addWidget(&tetraMeshMethodLabel);
  optionsLayout.addWidget(&tetraMeshMethodComboBox);
  optionsLayout.addWidget(&sofaTetraStuffingWidget);
  optionsLayout.addStretch();
  surfaceVisWidget.setMinimumSize(230, 160);
  //optionsLayout.addWidget(&surfaceOptionsFrame);
  centralWidget.setLayout(&mainLayout);
  mainLayout.addLayout(&viewerLayout);
  mainLayout.addLayout(&optionsLayout);
  centralLayout.addLayout(&mainLayout, 0, 0);
}

void QTetraMesherMainWindow::connectSlots() {
  // connect stuff to slots
  connect(&actionQuit, SIGNAL(triggered()), this, SLOT(close()));
  connect(&actionHelp, SIGNAL(triggered()), viewer, SLOT(help()));
  connect(&actionLoadSurface, SIGNAL(triggered()), viewer, SLOT(loadSurface()));
  connect(&actionLoadSurface, SIGNAL(triggered()), this, SLOT(clearTetraOptions()));
  connect(&surfaceVisWidget.surfaceVisComboBox,
          SIGNAL(currentIndexChanged(int)),
          this,
          SLOT(surfaceVisChanged()));
  connect(&surfaceVisWidget.surfaceColorButton,
          SIGNAL(clicked()),
          this,
          SLOT(surfaceColorButtonSlot()));
  connect(&surfaceVisWidget.surfaceWireframeColorButton,
          SIGNAL(clicked()),
          this,
          SLOT(surfaceWireframeColorButtonSlot()));
  connect(&tetraVisWidget.tetraVisComboBox,
          SIGNAL(currentIndexChanged(int)),
          this,
          SLOT(tetraVisChangedSlot()));
  connect(&tetraVisWidget.tetraColorButton, SIGNAL(clicked()), this, SLOT(tetraColorButtonSlot()));
  connect(&tetraVisWidget.tetraWireframeColorButton, SIGNAL(clicked()), this, SLOT(tetraWirefraceColorButtonSlot()));
  connect(&tetraVisWidget.tetraCutplaneSlider, SIGNAL(valueChanged(int)), this, SLOT(cutplaneSliderSlot()));
}

void QTetraMesherMainWindow::surfaceButtonSlot()
{
  std::cout << "HUIIIIII! SurfaceVisButton!" << std::endl;
}

void QTetraMesherMainWindow::surfaceVisChanged()
{
  int selectedIndex = surfaceVisWidget.surfaceVisComboBox.currentIndex();
  viewer->ToggleTriangleVis(selectedIndex);
}

void QTetraMesherMainWindow::resizeEvent(QResizeEvent *event) {
  viewer->resize(viewerFrame.size());
}

void QTetraMesherMainWindow::surfaceColorButtonSlot() {
  viewer->selectSurfaceColor();
}

void QTetraMesherMainWindow::surfaceWireframeColorButtonSlot()
{
  viewer->selectSurfaceColorWireframe();
}

void QTetraMesherMainWindow::tetraVisChangedSlot()
{
  int selectedIndex = tetraVisWidget.tetraVisComboBox.currentIndex();
  viewer->ToggleTetraVis(selectedIndex);
}

void QTetraMesherMainWindow::tetraColorButtonSlot()
{
  viewer->selectTetraColor();
}

void QTetraMesherMainWindow::tetraWirefraceColorButtonSlot()
{
  viewer->selectTetraColorWireframe();
}

void QTetraMesherMainWindow::cutplaneSliderSlot() {
  viewer->SetCutPlane(tetraVisWidget.tetraCutplaneSlider.value());
}

void QTetraMesherMainWindow::toggleFullScreen(bool value) {
  if (value) {
    this->showFullScreen();
  } else {
    this->showNormal();
  }
}

void QTetraMesherMainWindow::showTetraStuffingDialog() {
  if (tsd == NULL) {
    tsd = new TetraStuffingDialog();
    // tsd->setViewer(ui->viewer);
    connect(tsd, SIGNAL(displayMessage(QString,uint)), this,
            SLOT(displayMessage(QString,uint)));
    connect(tsd, SIGNAL(notifyDone(QString,uint)), this,
            SLOT(notifyDone(QString,uint)));
    // tsd->setTetraSize(ui->viewer->getMaxBBox() * 0.05f);
  }
  tsd->show();
}

void QTetraMesherMainWindow::showQuartetTetraDialog() {
  if (cts == NULL) {
    cts = new QuartetTetraStuffingDialog();
    // cts->setViewer(ui->viewer);
    connect(cts, SIGNAL(displayMessage(QString,uint)), this,
            SLOT(displayMessage(QString,uint)));
    connect(cts, SIGNAL(notifyDone(QString,uint)), this,
            SLOT(notifyDone(QString,uint)));
  }
  cts->show();
}

void QTetraMesherMainWindow::showCGALDialog() {
  if (ctd == NULL) {
    ctd = new CGALTetrahedralizeDialog();
    // ctd->setViewer(ui->viewer);
    connect(ctd, SIGNAL(displayMessage(QString,uint)), this,
            SLOT(displayMessage(QString,uint)));
    connect(ctd, SIGNAL(notifyDone(QString,uint)), this,
            SLOT(notifyDone(QString,uint)));
    // ctd->setParams(ui->viewer->getMaxBBox() * 0.1f,
    //                    ui->viewer->getMaxBBox() * 0.05);
  }
  ctd->show();
}

void QTetraMesherMainWindow::showTetgenDialog() {
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

void QTetraMesherMainWindow::generateOctree() {
  // ui->viewer->generateOctree(ui->octreeDepthSpinbox->value());
  //  ui->viewer->updateGL();
}

void QTetraMesherMainWindow::displayMessage(QString msg_, unsigned int time_) {
  // ui->viewer->ShowStatusMessage(msg_, time_);
}

void QTetraMesherMainWindow::notifyDone(QString msg_, unsigned int time_) {
  // ui->viewer->ShowStatusMessage(msg_, time_);
  //  ui->viewer->updateGL();
}

void QTetraMesherMainWindow::clearTetraOptions() {
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

void QTetraMesherMainWindow::closeEvent(QCloseEvent *event) {
  clearTetraOptions();
}

void QTetraMesherMainWindow::on_viewer_onLoad() {}
