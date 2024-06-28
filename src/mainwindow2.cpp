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

#include <QFrame>
#include <QMessageBox>
#include <QUrl>
#include <iostream>

QTetraMesherMainWindow::QTetraMesherMainWindow(QWidget *parent)
    : QMainWindow(parent),
      surfaceVisWidget(this),
      tetraVisWidget(this),
      octreeVisWidget(this),
      sofaTetraStuffingWidget(this),
      cgalTetrahedralizeWidget(this) {
  // setlocale(LC_NUMERIC, "C");
  setupUI();
  connectSlots();
  selectedTetraMethod = 0;
}

void QTetraMesherMainWindow::setupUI() {
  QFont boldFont;
  boldFont.setBold(true);
  this->setSizePolicy(QSizePolicy::MinimumExpanding,
                      QSizePolicy::MinimumExpanding);
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
  actionShowAxis.setText("Show Axis");
  actionShowGrid.setText("Show Grid");
  actionShowBBox.setText("Show BBox");
  actionShowAxis.setCheckable(true);
  actionShowGrid.setCheckable(true);
  actionShowBBox.setCheckable(true);
  actionShowAxis.setChecked(true);
  actionShowGrid.setChecked(true);
  viewMenu.addAction(&actionShowAxis);
  viewMenu.addAction(&actionShowGrid);
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
  centralWidget.setSizePolicy(QSizePolicy::Expanding,
                              QSizePolicy::MinimumExpanding);
  viewerFrame.setMaximumSize(820, 760);
  viewerFrame.setMaximumSize(3840, 2160);
  viewerFrame.setSizePolicy(QSizePolicy::Expanding,
                            QSizePolicy::MinimumExpanding);
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
  surfaceOptionsFrame.setSizePolicy(QSizePolicy::Expanding,
                                    QSizePolicy::MinimumExpanding);
  surfaceOptionsFrame.setFrameShape(QFrame::Box);
  surfaceOptionsFrame.setFrameShadow(QFrame::Raised);
  tetraMeshMethodLabel.setText("Tetra Mesh Method");
  tetraMeshMethodLabel.setFont(boldFont);
  tetraMeshMethodComboBox.addItem("SOFA TetraStuffing");
  tetraMeshMethodComboBox.addItem("CGAL Tetrahedralize");
  tetraMeshMethodComboBox.addItem("Quartet TetraStuffing");
  tetraMeshMethodComboBox.addItem("Tetgen TetraStuffing");
  optionsLayout.addWidget(&surfaceVisWidget);
  QFrame f1;
  f1.setFrameShape(QFrame::HLine);
  f1.setFrameShadow(QFrame::Sunken);
  optionsLayout.addWidget(&f1);
  optionsLayout.addWidget(&tetraVisWidget);
  QFrame f2;
  f2.setFrameShape(QFrame::HLine);
  f2.setFrameShadow(QFrame::Sunken);
  optionsLayout.addWidget(&f2);
  optionsLayout.addWidget(&octreeVisWidget);
  optionsLayout.addWidget(&tetraMeshMethodLabel);
  optionsLayout.addWidget(&tetraMeshMethodComboBox);
  optionsLayout.addWidget(&sofaTetraStuffingWidget);
  optionsLayout.addWidget(&cgalTetrahedralizeWidget);
  cgalTetrahedralizeWidget.hide();
  optionsLayout.addStretch();
  surfaceVisWidget.setMinimumSize(230, 160);

  centralWidget.setLayout(&mainLayout);
  mainLayout.addLayout(&viewerLayout);
  mainLayout.addLayout(&optionsLayout);
  centralLayout.addLayout(&mainLayout, 0, 0);
}

void QTetraMesherMainWindow::connectSlots() {
  // connect stuff to slots
  connect(&actionQuit, SIGNAL(triggered()), this, SLOT(close()));
  connect(&actionHelp, SIGNAL(triggered()), viewer, SLOT(help()));
  connect(&actionAbout, SIGNAL(triggered()), viewer, SLOT(aboutQGLViewer()));
  connect(&actionLoadSurface, SIGNAL(triggered()), this,
          SLOT(loadSurfaceSlot()));
  connect(&actionLoadSurface, SIGNAL(triggered()), this,
          SLOT(clearTetraOptions()));
  connect(&surfaceVisWidget.surfaceVisComboBox,
          SIGNAL(currentIndexChanged(int)), this, SLOT(surfaceVisChanged()));
  connect(&surfaceVisWidget.surfaceColorButton, SIGNAL(clicked()), this,
          SLOT(surfaceColorButtonSlot()));
  connect(&surfaceVisWidget.surfaceWireframeColorButton, SIGNAL(clicked()),
          this, SLOT(surfaceWireframeColorButtonSlot()));
  connect(&tetraVisWidget.tetraVisComboBox, SIGNAL(currentIndexChanged(int)),
          this, SLOT(tetraVisChangedSlot()));
  connect(&tetraVisWidget.tetraColorButton, SIGNAL(clicked()), this,
          SLOT(tetraColorButtonSlot()));
  connect(&tetraVisWidget.tetraWireframeColorButton, SIGNAL(clicked()), this,
          SLOT(tetraWirefraceColorButtonSlot()));
  connect(&tetraVisWidget.tetraCutplaneSlider, SIGNAL(valueChanged(int)), this,
          SLOT(cutplaneSliderSlot()));
  connect(&sofaTetraStuffingWidget.generateTetrahedraButton, SIGNAL(clicked()),
          this, SLOT(generateSofaTetraStuffingSlot()));
  connect(&cgalTetrahedralizeWidget.generateTetrahedraButton, SIGNAL(clicked()),
          this, SLOT(generateCGALTetrahedralizeSlot()));
  // View Menu
  connect(&actionShowAxis, SIGNAL(changed()), this, SLOT(setAxisShownSlot()));
  connect(&actionShowGrid, SIGNAL(changed()), this, SLOT(setGridShownSlot()));
  connect(&actionShowBBox, SIGNAL(changed()), this, SLOT(setBBoxVisibleSlot()));
  connect(&octreeVisWidget.octreeGenerateButton, SIGNAL(clicked()), this,
          SLOT(generateOctreeSlot()));
  connect(&octreeVisWidget.octreeVisComboBox, SIGNAL(currentIndexChanged(int)),
          this, SLOT(octreeVisChangedSlot()));
  connect(&tetraMeshMethodComboBox, SIGNAL(currentIndexChanged(int)), this,
          SLOT(tetraMethodComboBoxSlot()));
}

// visualization slots
void QTetraMesherMainWindow::setAxisShownSlot() {
  viewer->setAxisIsDrawn(actionShowAxis.isChecked());
}

void QTetraMesherMainWindow::setGridShownSlot() {
  viewer->setGridIsDrawn(actionShowGrid.isChecked());
}

void QTetraMesherMainWindow::setBBoxVisibleSlot() {
  viewer->tMesh->ToggleBBox(actionShowBBox.isChecked());
}

void QTetraMesherMainWindow::surfaceVisChanged() {
  int selectedIndex = surfaceVisWidget.surfaceVisComboBox.currentIndex();
  viewer->ToggleTriangleVis(selectedIndex);
}

void QTetraMesherMainWindow::resizeEvent(QResizeEvent *event) {
  viewer->resize(viewerFrame.size());
}

void QTetraMesherMainWindow::surfaceColorButtonSlot() {
  viewer->selectSurfaceColor();
}

void QTetraMesherMainWindow::surfaceWireframeColorButtonSlot() {
  viewer->selectSurfaceColorWireframe();
}

void QTetraMesherMainWindow::tetraVisChangedSlot() {
  int selectedIndex = tetraVisWidget.tetraVisComboBox.currentIndex();
  viewer->ToggleTetraVis(selectedIndex);
}

void QTetraMesherMainWindow::tetraColorButtonSlot() {
  viewer->selectTetraColor();
}

void QTetraMesherMainWindow::tetraWirefraceColorButtonSlot() {
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

// menu + method slots
void QTetraMesherMainWindow::tetraMethodComboBoxSlot() {
  int newSelectedMethod = tetraMeshMethodComboBox.currentIndex();
  // replace method options here
  cgalTetrahedralizeWidget.hide();
  sofaTetraStuffingWidget.hide();
  switch (newSelectedMethod) {
    case 0:
      cgalTetrahedralizeWidget.hide();
      sofaTetraStuffingWidget.show();
      break;
    case 1:
      cgalTetrahedralizeWidget.show();
      sofaTetraStuffingWidget.hide();
      break;
    case 2:
      break;
    case 3:
      break;
    default:
      break;
  }
  selectedTetraMethod = tetraMeshMethodComboBox.currentIndex();
}

void QTetraMesherMainWindow::loadSurfaceSlot() {
  viewer->loadSurface();
  sofaTetraStuffingWidget.tetraSizeSpinBox.setValue(viewer->getMaxBBox() *
                                                    0.05f);
  cgalTetrahedralizeWidget.cellSizeSpinBox.setValue(viewer->getMaxBBox() * 0.1);
  cgalTetrahedralizeWidget.facetSizeSpinBox.setValue(viewer->getMaxBBox() *
                                                     0.05);
  // viewer->tMesh->GetSurface()->GenerateBoundingBox();
}

void QTetraMesherMainWindow::generateSofaTetraStuffingSlot() {
  sofaTetraStuffingWidget.generateTetrahedra(viewer);
}

void QTetraMesherMainWindow::generateCGALTetrahedralizeSlot() {
  cgalTetrahedralizeWidget.generateTetrahedra(viewer);
}

void QTetraMesherMainWindow::showTetraStuffingDialog() {
  if (tsd == NULL) {
    tsd = new TetraStuffingDialog();
    // tsd->setViewer(ui->viewer);
    connect(tsd, SIGNAL(displayMessage(QString, uint)), this,
            SLOT(displayMessage(QString, uint)));
    connect(tsd, SIGNAL(notifyDone(QString, uint)), this,
            SLOT(notifyDone(QString, uint)));
    // tsd->setTetraSize(ui->viewer->getMaxBBox() * 0.05f);
  }
  tsd->show();
}

void QTetraMesherMainWindow::showQuartetTetraDialog() {
  if (cts == NULL) {
    cts = new QuartetTetraStuffingDialog();
    // cts->setViewer(ui->viewer);
    connect(cts, SIGNAL(displayMessage(QString, uint)), this,
            SLOT(displayMessage(QString, uint)));
    connect(cts, SIGNAL(notifyDone(QString, uint)), this,
            SLOT(notifyDone(QString, uint)));
  }
  cts->show();
}

void QTetraMesherMainWindow::showCGALDialog() {
  if (ctd == NULL) {
    ctd = new CGALTetrahedralizeDialog();
    // ctd->setViewer(ui->viewer);
    connect(ctd, SIGNAL(displayMessage(QString, uint)), this,
            SLOT(displayMessage(QString, uint)));
    connect(ctd, SIGNAL(notifyDone(QString, uint)), this,
            SLOT(notifyDone(QString, uint)));
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

void QTetraMesherMainWindow::generateOctreeSlot() {
  viewer->generateOctree(octreeVisWidget.octreeDepthSpinBox.value());
  viewer->tMesh->Draw();
  viewer->update();
}

void QTetraMesherMainWindow::octreeVisChangedSlot() {
  viewer->ToggleOctreeVis(octreeVisWidget.octreeVisComboBox.currentIndex());
}

void QTetraMesherMainWindow::displayMessage(QString msg_, unsigned int time_) {
  viewer->ShowStatusMessage(msg_, time_);
}

void QTetraMesherMainWindow::notifyDone(QString msg_, unsigned int time_) {
  viewer->ShowStatusMessage(msg_, time_);
  viewer->update();
}

void QTetraMesherMainWindow::closeEvent(QCloseEvent *event) {}

void QTetraMesherMainWindow::on_viewer_onLoad() {}
