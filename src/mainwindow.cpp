/*
 * mainwindow.cpp
 *
 *  Created 2012
 *      Author: Dennis Luebke
 */

#include "mainwindow.hpp"

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
      cgalTetrahedralizeWidget(this),
      quartetTetraStuffingWidget(this),
      tetgenWidget(this) {
  // setlocale(LC_NUMERIC, "C");
  setupUI();
  connectSlots();
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
  tetraMeshMethodComboBox.addItem("Tetgen Tetrahedralize");
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
  optionsLayout.addWidget(&quartetTetraStuffingWidget);
  optionsLayout.addWidget(&tetgenWidget);
  cgalTetrahedralizeWidget.hide();
  quartetTetraStuffingWidget.hide();
  tetgenWidget.hide();
  optionsLayout.addStretch();
  surfaceVisWidget.setMinimumSize(230, 160);

  centralWidget.setLayout(&mainLayout);
  mainLayout.addLayout(&viewerLayout);
  mainLayout.addLayout(&optionsLayout);
  centralLayout.addLayout(&mainLayout, 0, 0);
  // set color button colors
  const QColor surfaceColor = viewer->tMesh->GetSurfaceColor();
  surfaceVisWidget.surfaceColorButton.setStyleSheet(
      "QPushButton{background-color: rgb(" +
      QString::number(surfaceColor.red()) + "," +
      QString::number(surfaceColor.green()) + "," +
      QString::number(surfaceColor.blue()) + ");}\n");
  const QColor wireframeeColor = viewer->tMesh->GetSurfaceWireframeColor();
  surfaceVisWidget.surfaceWireframeColorButton.setStyleSheet(
      "QPushButton{background-color: rgb(" +
      QString::number(wireframeeColor.red()) + "," +
      QString::number(wireframeeColor.green()) + "," +
      QString::number(wireframeeColor.blue()) + ");}\n");
  const QColor tetraColor = viewer->tMesh->GetTetraColor();
  tetraVisWidget.tetraColorButton.setStyleSheet(
      "QPushButton{background-color: rgb(" + QString::number(tetraColor.red()) +
      "," + QString::number(tetraColor.green()) + "," +
      QString::number(tetraColor.blue()) + ");}\n");
  const QColor tetraWireframeColor = viewer->tMesh->GetTetraWireframeColor();
  tetraVisWidget.tetraWireframeColorButton.setStyleSheet(
      "QPushButton{background-color: rgb(" +
      QString::number(tetraWireframeColor.red()) + "," +
      QString::number(tetraWireframeColor.green()) + "," +
      QString::number(tetraWireframeColor.blue()) + ");}\n");
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
  connect(&actionExportSurface, SIGNAL(triggered()), this,
          SLOT(exportSurfaceSlot()));
  connect(&actionLoadGMSH, SIGNAL(triggered()), this, SLOT(loadGMSHSlot()));
  connect(&actionExportGMSH, SIGNAL(triggered()), this, SLOT(exportGMSHSlot()));
  connect(&surfaceVisWidget.surfaceVisComboBox,
          SIGNAL(currentIndexChanged(int)), this, SLOT(surfaceVisChanged()));
  connect(&surfaceVisWidget.surfaceColorButton, SIGNAL(clicked()), this,
          SLOT(surfaceColorButtonSlot()));
  connect(&surfaceVisWidget.surfaceWireframeColorButton, SIGNAL(clicked()),
          this, SLOT(surfaceWireframeColorButtonSlot()));
  connect(&tetraVisWidget.drawWireframeCheckbox, SIGNAL(toggled(bool)), this,
          SLOT(tetraVisChangedSlot()));
  connect(&tetraVisWidget.drawSolidCheckbox, SIGNAL(toggled(bool)), this,
          SLOT(tetraVisChangedSlot()));
  connect(&tetraVisWidget.tetraColorButton, SIGNAL(clicked()), this,
          SLOT(tetraColorButtonSlot()));
  connect(&tetraVisWidget.tetraWireframeColorButton, SIGNAL(clicked()), this,
          SLOT(tetraWirefraceColorButtonSlot()));
  connect(&tetraVisWidget.tetraXCutplaneSlider, SIGNAL(lowerValueChanged(int)),
          this, SLOT(cutplaneXSliderSlot()));
  connect(&tetraVisWidget.tetraXCutplaneSlider, SIGNAL(upperValueChanged(int)),
          this, SLOT(cutplaneXSliderSlot()));
  connect(&tetraVisWidget.tetraYCutplaneSlider, SIGNAL(lowerValueChanged(int)),
          this, SLOT(cutplaneYSliderSlot()));
  connect(&tetraVisWidget.tetraYCutplaneSlider, SIGNAL(upperValueChanged(int)),
          this, SLOT(cutplaneYSliderSlot()));
  connect(&tetraVisWidget.tetraZCutplaneSlider, SIGNAL(lowerValueChanged(int)),
          this, SLOT(cutplaneZSliderSlot()));
  connect(&tetraVisWidget.tetraZCutplaneSlider, SIGNAL(upperValueChanged(int)),
          this, SLOT(cutplaneZSliderSlot()));
  connect(&sofaTetraStuffingWidget.generateTetrahedraButton, SIGNAL(clicked()),
          this, SLOT(generateSofaTetraStuffingSlot()));
  connect(&cgalTetrahedralizeWidget.generateTetrahedraButton, SIGNAL(clicked()),
          this, SLOT(generateCGALTetrahedralizeSlot()));
  connect(&quartetTetraStuffingWidget.generateTetrahedraButton,
          SIGNAL(clicked()), this, SLOT(generateQuartetTetraStuffingSlot()));
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
  const QColor surfaceColor = viewer->tMesh->GetSurfaceColor();
  surfaceVisWidget.surfaceColorButton.setStyleSheet(
      "QPushButton{background-color: rgb(" +
      QString::number(surfaceColor.red()) + "," +
      QString::number(surfaceColor.green()) + "," +
      QString::number(surfaceColor.blue()) + ");}\n");
}

void QTetraMesherMainWindow::surfaceWireframeColorButtonSlot() {
  viewer->selectSurfaceColorWireframe();
  const QColor wireframeeColor = viewer->tMesh->GetSurfaceWireframeColor();
  surfaceVisWidget.surfaceWireframeColorButton.setStyleSheet(
      "QPushButton{background-color: rgb(" +
      QString::number(wireframeeColor.red()) + "," +
      QString::number(wireframeeColor.green()) + "," +
      QString::number(wireframeeColor.blue()) + ");}\n");
}

void QTetraMesherMainWindow::tetraVisChangedSlot() {
  viewer->ToggleTetraVis(tetraVisWidget.drawWireframeCheckbox.isChecked(),
                         tetraVisWidget.drawSolidCheckbox.isChecked());
}

void QTetraMesherMainWindow::tetraColorButtonSlot() {
  viewer->selectTetraColor();
  const QColor tetraColor = viewer->tMesh->GetTetraColor();
  tetraVisWidget.tetraColorButton.setStyleSheet(
      "QPushButton{background-color: rgb(" + QString::number(tetraColor.red()) +
      "," + QString::number(tetraColor.green()) + "," +
      QString::number(tetraColor.blue()) + ");}\n");
}

void QTetraMesherMainWindow::tetraWirefraceColorButtonSlot() {
  viewer->selectTetraColorWireframe();
  const QColor tetraWireframeColor = viewer->tMesh->GetTetraWireframeColor();
  tetraVisWidget.tetraWireframeColorButton.setStyleSheet(
      "QPushButton{background-color: rgb(" +
      QString::number(tetraWireframeColor.red()) + "," +
      QString::number(tetraWireframeColor.green()) + "," +
      QString::number(tetraWireframeColor.blue()) + ");}\n");
}

void QTetraMesherMainWindow::cutplaneXSliderSlot() {
  viewer->tMesh->SetXRange(tetraVisWidget.tetraXCutplaneSlider.GetLowerValue(),
                           tetraVisWidget.tetraXCutplaneSlider.GetUpperValue());
  viewer->tMesh->Draw();
  viewer->update();
}

void QTetraMesherMainWindow::cutplaneYSliderSlot() {
  viewer->tMesh->SetYRange(tetraVisWidget.tetraYCutplaneSlider.GetLowerValue(),
                           tetraVisWidget.tetraYCutplaneSlider.GetUpperValue());
  viewer->tMesh->Draw();
  viewer->update();
}

void QTetraMesherMainWindow::cutplaneZSliderSlot() {
  viewer->tMesh->SetZRange(tetraVisWidget.tetraZCutplaneSlider.GetLowerValue(),
                           tetraVisWidget.tetraZCutplaneSlider.GetUpperValue());
  viewer->tMesh->Draw();
  viewer->update();
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
  // hide allexport
  cgalTetrahedralizeWidget.hide();
  sofaTetraStuffingWidget.hide();
  quartetTetraStuffingWidget.hide();
  tetgenWidget.hide();
  // show only selected
  switch (newSelectedMethod) {
    case 0:
      sofaTetraStuffingWidget.show();
      break;
    case 1:
      cgalTetrahedralizeWidget.show();
      break;
    case 2:
      quartetTetraStuffingWidget.show();
      break;
    case 3:
      tetgenWidget.show();
      break;
    default:
      break;
  }
}

void QTetraMesherMainWindow::loadSurfaceSlot() {
  viewer->loadSurface();
  sofaTetraStuffingWidget.tetraSizeSpinBox.setValue(viewer->getMaxBBox() *
                                                    0.05f);
  cgalTetrahedralizeWidget.cellSizeSpinBox.setValue(viewer->getMaxBBox() * 0.1);
  cgalTetrahedralizeWidget.facetSizeSpinBox.setValue(viewer->getMaxBBox() *
                                                     0.05);
  quartetTetraStuffingWidget.cellSizeSpinBox.setValue(viewer->getMaxBBox() *
                                                      0.2);
}

void QTetraMesherMainWindow::exportSurfaceSlot() { viewer->saveSurface(); }

void QTetraMesherMainWindow::exportGMSHSlot() { viewer->saveGMSH(); }

void QTetraMesherMainWindow::loadGMSHSlot() {
  viewer->loadGMSH();
  sofaTetraStuffingWidget.tetraSizeSpinBox.setValue(viewer->getMaxBBox() *
                                                    0.05f);
  cgalTetrahedralizeWidget.cellSizeSpinBox.setValue(viewer->getMaxBBox() * 0.1);
  cgalTetrahedralizeWidget.facetSizeSpinBox.setValue(viewer->getMaxBBox() *
                                                     0.05);
  quartetTetraStuffingWidget.cellSizeSpinBox.setValue(viewer->getMaxBBox() *
                                                      0.2);
  updateCutplaneSliders();
}

void QTetraMesherMainWindow::updateCutplaneSliders() {
  tetraVisWidget.tetraXCutplaneSlider.setLowerValue(1);
  tetraVisWidget.tetraXCutplaneSlider.setUpperValue(100);
  tetraVisWidget.tetraYCutplaneSlider.setLowerValue(1);
  tetraVisWidget.tetraYCutplaneSlider.setUpperValue(100);
  tetraVisWidget.tetraZCutplaneSlider.setLowerValue(1);
  tetraVisWidget.tetraZCutplaneSlider.setUpperValue(100);
  viewer->tMesh->SetXRange(1, 100);
  viewer->tMesh->SetYRange(1, 100);
  viewer->tMesh->SetZRange(1, 100);
}

void QTetraMesherMainWindow::generateSofaTetraStuffingSlot() {
  sofaTetraStuffingWidget.generateTetrahedra(viewer);
  updateCutplaneSliders();
}

void QTetraMesherMainWindow::generateCGALTetrahedralizeSlot() {
  cgalTetrahedralizeWidget.generateTetrahedra(viewer);
  updateCutplaneSliders();
}

void QTetraMesherMainWindow::generateQuartetTetraStuffingSlot() {
  quartetTetraStuffingWidget.generateTetrahedra(viewer);
  updateCutplaneSliders();
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
