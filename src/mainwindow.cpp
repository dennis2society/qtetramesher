/*
 * mainwindow.cpp
 *
 *  Created 2012
 *      Author: Dennis Luebke
 */

#include "mainwindow.hpp"

#include "QMimeData"
// #include "ui_mainwindow.h"

#include <QApplication>
#include <QSettings>
#include <QtConcurrent/QtConcurrent>
#include <QFileInfo>
#include <QFrame>
#include <QMessageBox>
#include <QUrl>
#include <TetraMeshTools/SurfaceChecker.h>
#include <iostream>
#include <sstream>

QTetraMesherMainWindow::QTetraMesherMainWindow(QWidget *parent)
    : QMainWindow(parent),
      surfaceVisWidget(this),
      tetraVisWidget(this),
      octreeVisWidget(this),
      sofaTetraStuffingWidget(this),
      cgalTetrahedralizeWidget(this),
      genericIsosurfaceStuffingWidget(this),
      tetgenWidget(this),
      gmshWidget(this),
      ftetwildWidget(this) {
  // setlocale(LC_NUMERIC, "C");
  setupUI();
  connectSlots();
  updateWindowTitle();
  // after connectSlots() so that restored options are applied to the viewer
  loadSettings();
}

QTetraMesherMainWindow::~QTetraMesherMainWindow() {
  // the background threads read the mesh owned by the viewer
  stopBackgroundTasks();
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
  actionLoadTetgen.setText("Load Tetgen (.node/.ele)");
  fileMenu.addAction(&actionLoadTetgen);
  fileMenu.addSeparator();
  actionExportGMSH.setText("Export GMSH");
  fileMenu.addAction(&actionExportGMSH);
  actionExportTetgen.setText("Export Tetgen (.node/.ele)");
  fileMenu.addAction(&actionExportTetgen);
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
  viewer->setMinimumSize(820, 780);
  viewer->setMaximumSize(3840, 2160);
  viewer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
  // the layout (owned by the frame) makes the viewer follow the frame size
  auto *viewerFrameLayout = new QVBoxLayout(&viewerFrame);
  viewerFrameLayout->setContentsMargins(0, 0, 0, 0);
  viewerFrameLayout->addWidget(viewer);
  viewerLayout.addWidget(&viewerFrame);
  surfaceOptionsFrame.setSizePolicy(QSizePolicy::Expanding,
                                    QSizePolicy::MinimumExpanding);
  surfaceOptionsFrame.setFrameShape(QFrame::Box);
  surfaceOptionsFrame.setFrameShadow(QFrame::Raised);
  tetraMeshMethodLabel.setText("Tetra Mesh Method");
  tetraMeshMethodLabel.setFont(boldFont);
  tetraMeshMethodComboBox.addItem("SOFA TetraStuffing");
  tetraMeshMethodComboBox.addItem("CGAL Tetrahedralize");
  tetraMeshMethodComboBox.addItem("Generic Isosurface Stuffing");
  tetraMeshMethodComboBox.addItem("Tetgen Tetrahedralize");
  tetraMeshMethodComboBox.addItem("GMSH Tetrahedralize");
#ifdef QTM_HAVE_FTETWILD
  tetraMeshMethodComboBox.addItem("fTetWild Tetrahedralize");
#endif
  repairOnLoadCheckBox.setText("Repair surface mesh on load");
  repairOnLoadCheckBox.setToolTip(
      "Merge duplicate vertices, remove degenerate and duplicate triangles, "
      "fix the orientation and fill holes when loading a surface mesh");
  repairOnLoadCheckBox.setChecked(true);
  optionsLayout.addWidget(&repairOnLoadCheckBox);
  optionsLayout.addWidget(&surfaceVisWidget);
  // owned by this window (a local QFrame would be destroyed on return)
  QFrame *f1 = new QFrame(this);
  f1->setFrameShape(QFrame::HLine);
  f1->setFrameShadow(QFrame::Sunken);
  optionsLayout.addWidget(f1);
  optionsLayout.addWidget(&tetraVisWidget);
  QFrame *f2 = new QFrame(this);
  f2->setFrameShape(QFrame::HLine);
  f2->setFrameShadow(QFrame::Sunken);
  optionsLayout.addWidget(f2);
  optionsLayout.addWidget(&tetraMeshMethodLabel);
  optionsLayout.addWidget(&tetraMeshMethodComboBox);
  optionsLayout.addWidget(&sofaTetraStuffingWidget);
  optionsLayout.addWidget(&cgalTetrahedralizeWidget);
  optionsLayout.addWidget(&genericIsosurfaceStuffingWidget);
  optionsLayout.addWidget(&tetgenWidget);
  optionsLayout.addWidget(&gmshWidget);
  optionsLayout.addWidget(&ftetwildWidget);
  QFrame *f3 = new QFrame(this);
  f3->setFrameShape(QFrame::HLine);
  f3->setFrameShadow(QFrame::Sunken);
  optionsLayout.addWidget(f3);
  optionsLayout.addWidget(&octreeVisWidget);
  cgalTetrahedralizeWidget.hide();
  genericIsosurfaceStuffingWidget.hide();
  tetgenWidget.hide();
  gmshWidget.hide();
  ftetwildWidget.hide();
  optionsLayout.addStretch();
  surfaceVisWidget.setMinimumSize(230, 190);

  // centralLayout is the layout of centralWidget, mainLayout goes into it
  // Minimum size policy: scroll instead of squeezing the options widgets
  optionsContainer.setLayout(&optionsLayout);
  optionsContainer.setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  optionsScrollArea.setWidget(&optionsContainer);
  optionsScrollArea.setWidgetResizable(true);
  optionsScrollArea.setFrameShape(QFrame::NoFrame);
  optionsScrollArea.setSizePolicy(QSizePolicy::Minimum,
                                  QSizePolicy::Expanding);
  mainLayout.addLayout(&viewerLayout);
  mainLayout.addWidget(&optionsScrollArea);
  centralLayout.addLayout(&mainLayout, 0, 0);
  updateColorButtons();
}

void QTetraMesherMainWindow::connectSlots() {
  // connect stuff to slots
  connect(&actionQuit, SIGNAL(triggered()), this, SLOT(close()));
  connect(&actionHelp, SIGNAL(triggered()), viewer, SLOT(showHelp()));
  connect(&actionAbout, SIGNAL(triggered()), viewer, SLOT(showAbout()));
  // keep the menu in sync with the viewer's keyboard shortcuts
  connect(viewer, SIGNAL(axisIsDrawnChanged(bool)), &actionShowAxis,
          SLOT(setChecked(bool)));
  connect(viewer, SIGNAL(gridIsDrawnChanged(bool)), &actionShowGrid,
          SLOT(setChecked(bool)));
  connect(&actionLoadSurface, SIGNAL(triggered()), this,
          SLOT(loadSurfaceSlot()));
  connect(&actionExportSurface, SIGNAL(triggered()), this,
          SLOT(exportSurfaceSlot()));
  connect(&actionLoadGMSH, SIGNAL(triggered()), this, SLOT(loadGMSHSlot()));
  connect(&actionExportGMSH, SIGNAL(triggered()), this, SLOT(exportGMSHSlot()));
  connect(&actionLoadTetgen, SIGNAL(triggered()), this, SLOT(loadTetgenSlot()));
  connect(&actionExportTetgen, SIGNAL(triggered()), this, SLOT(exportTetgenSlot()));
  connect(&surfaceVisWidget.surfaceVisComboBox,
          SIGNAL(currentIndexChanged(int)), this, SLOT(surfaceVisChanged()));
  connect(&surfaceVisWidget.surfaceColorButton, SIGNAL(clicked()), this,
          SLOT(surfaceColorButtonSlot()));
  connect(&surfaceVisWidget.surfaceWireframeColorButton, SIGNAL(clicked()),
          this, SLOT(surfaceWireframeColorButtonSlot()));
  connect(&repairOnLoadCheckBox, &QCheckBox::toggled, viewer,
          &QGLTetraViewer::setRepairOnLoad);
  connect(&surfaceVisWidget.checkSurfaceButton, SIGNAL(clicked()), this,
          SLOT(checkSurfaceSlot()));
  connect(&tetraVisWidget.drawWireframeCheckbox, SIGNAL(toggled(bool)), this,
          SLOT(tetraVisChangedSlot()));
  connect(&tetraVisWidget.drawSolidCheckbox, SIGNAL(toggled(bool)), this,
          SLOT(tetraVisChangedSlot()));
  connect(&tetraVisWidget.tetraColorButton, SIGNAL(clicked()), this,
          SLOT(tetraColorButtonSlot()));
  connect(&tetraVisWidget.tetraOuterColorButton, SIGNAL(clicked()), this,
          SLOT(tetraOuterColorButtonSlot()));
  connect(&tetraVisWidget.tetraInnerColorButton, SIGNAL(clicked()), this,
          SLOT(tetraInnerColorButtonSlot()));
  connect(&tetraVisWidget.depthColoringCheckbox, SIGNAL(toggled(bool)), this,
          SLOT(depthColoringSlot(bool)));
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
  connect(&genericIsosurfaceStuffingWidget.generateTetrahedraButton,
          SIGNAL(clicked()), this, SLOT(generateGenericIsosurfaceStuffingSlot()));
  connect(&tetgenWidget.generateTetrahedraButton, SIGNAL(clicked()), this,
          SLOT(generateTetgenSlot()));
  connect(&gmshWidget.generateTetrahedraButton, SIGNAL(clicked()), this,
          SLOT(generateGMSHSlot()));
  connect(&ftetwildWidget.generateTetrahedraButton, SIGNAL(clicked()), this,
          SLOT(generateFTetWildSlot()));
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
  // background tasks
  connect(&tetraJob, &TetraJob::finished, this,
          &QTetraMesherMainWindow::tetraJobFinished);
  connect(&octreeWatcher, &QFutureWatcher<Octree *>::finished, this,
          &QTetraMesherMainWindow::octreeFinished);
  connect(&surfaceCheckWatcher,
          &QFutureWatcher<std::shared_ptr<SurfaceCheckResult>>::finished, this,
          &QTetraMesherMainWindow::surfaceCheckFinished);
}

// visualization slots
void QTetraMesherMainWindow::setAxisShownSlot() {
  viewer->setAxisIsDrawn(actionShowAxis.isChecked());
}

void QTetraMesherMainWindow::setGridShownSlot() {
  viewer->setGridIsDrawn(actionShowGrid.isChecked());
}

void QTetraMesherMainWindow::setBBoxVisibleSlot() {
  viewer->ToggleBBox(actionShowBBox.isChecked());
}

void QTetraMesherMainWindow::surfaceVisChanged() {
  int selectedIndex = surfaceVisWidget.surfaceVisComboBox.currentIndex();
  viewer->ToggleTriangleVis(selectedIndex);
}

void QTetraMesherMainWindow::surfaceColorButtonSlot() {
  viewer->selectSurfaceColor();
  updateColorButtons();
}

void QTetraMesherMainWindow::surfaceWireframeColorButtonSlot() {
  viewer->selectSurfaceColorWireframe();
  updateColorButtons();
}

void QTetraMesherMainWindow::checkSurfaceSlot() {
  if (surfaceCheckWatcher.isRunning()) return;
  if (viewer->tMesh == NULL || viewer->tMesh->GetSurface() == NULL) {
    QMessageBox::information(this, "Check Surface Mesh",
                             "No surface mesh loaded.");
    return;
  }
  // loading is disabled while the check runs, so the data stays valid
  const std::vector<Vec3f> *verts = &viewer->tMesh->GetSurface()->GetVertices();
  const std::vector<Triangle> *tris =
      &viewer->tMesh->GetSurface()->GetTriangles();
  const std::atomic<bool> *stop = &stopThreads;
  viewer->beginTask("check", "Checking the surface mesh");
  surfaceCheckWatcher.setFuture(QtConcurrent::run([verts, tris, stop]() {
    try {
      return std::make_shared<SurfaceCheckResult>(
          CheckSurfaceMesh(*verts, *tris, stop));
    } catch (const std::exception &e) {
      std::cerr << "Surface check failed: " << e.what() << std::endl;
      return std::shared_ptr<SurfaceCheckResult>();
    }
  }));
  updateBusyState();
}

void QTetraMesherMainWindow::surfaceCheckFinished() {
  viewer->endTask("check");
  updateBusyState();
  std::shared_ptr<SurfaceCheckResult> r = surfaceCheckWatcher.result();
  if (r && r->canceled) return;
  if (!r) {
    QMessageBox::warning(this, "Check Surface Mesh",
                         "The surface check failed (out of memory?).");
    return;
  }
  showSurfaceCheckResult(*r);
}

void QTetraMesherMainWindow::showSurfaceCheckResult(
    const SurfaceCheckResult &r) {
  std::stringstream ss;
  ss << "Triangles: " << r.numTriangles << "\n"
     << "Intersecting triangle pairs: " << r.numIntersectingPairs << " ("
     << r.numIntersectingTriangles << " triangles)\n"
     << "Degenerate triangles: " << r.numDegenerateTriangles << "\n"
     << "Open (boundary) edges: " << r.numBoundaryEdges << "\n"
     << "Non-manifold edges: " << r.numNonManifoldEdges << "\n"
     << "Inconsistently oriented edges: " << r.numInconsistentEdges << "\n"
     << "Unused vertices: " << r.numUnusedVertices << "\n"
     << "Connected components: " << r.numComponents << "\n";
  if (r.genus >= 0) ss << "Genus: " << r.genus << "\n";
  ss << "\n";
  if (r.IsSuitable()) {
    ss << "The surface is closed, manifold and free of self-intersections.";
    if (r.genus > 0 || r.numComponents > 1) {
      ss << "\n\nNote: GMSH's surface parametrization can fail on surfaces "
            "with holes (genus > 0) or several components.";
    }
  } else {
    ss << "The surface is NOT suitable for Tetgen/GMSH volume meshing.";
    for (const auto &p : r.examplePairs) {
      std::cout << "Intersecting triangles: " << p.first << " / " << p.second
                << std::endl;
    }
  }
  const QString text = QString::fromStdString(ss.str());
  if (r.IsSuitable()) {
    QMessageBox::information(this, "Check Surface Mesh", text);
  } else {
    QMessageBox::warning(this, "Check Surface Mesh", text);
  }
}

void QTetraMesherMainWindow::tetraVisChangedSlot() {
  viewer->ToggleTetraVis(tetraVisWidget.drawWireframeCheckbox.isChecked(),
                         tetraVisWidget.drawSolidCheckbox.isChecked());
}

void QTetraMesherMainWindow::tetraColorButtonSlot() {
  viewer->selectTetraColor();
  updateColorButtons();
}

void QTetraMesherMainWindow::tetraOuterColorButtonSlot() {
  viewer->selectTetraOuterColor();
  updateColorButtons();
}

void QTetraMesherMainWindow::tetraInnerColorButtonSlot() {
  viewer->selectTetraInnerColor();
  updateColorButtons();
}

void QTetraMesherMainWindow::depthColoringSlot(bool enabled_) {
  viewer->tMesh->SetTetraDepthColoring(enabled_);
  viewer->update();
}

void QTetraMesherMainWindow::tetraWirefraceColorButtonSlot() {
  viewer->selectTetraColorWireframe();
  updateColorButtons();
}

void QTetraMesherMainWindow::cutplaneXSliderSlot() {
  viewer->tMesh->SetXRange(tetraVisWidget.tetraXCutplaneSlider.GetLowerValue(),
                           tetraVisWidget.tetraXCutplaneSlider.GetUpperValue());
  viewer->update();
}

void QTetraMesherMainWindow::cutplaneYSliderSlot() {
  viewer->tMesh->SetYRange(tetraVisWidget.tetraYCutplaneSlider.GetLowerValue(),
                           tetraVisWidget.tetraYCutplaneSlider.GetUpperValue());
  viewer->update();
}

void QTetraMesherMainWindow::cutplaneZSliderSlot() {
  viewer->tMesh->SetZRange(tetraVisWidget.tetraZCutplaneSlider.GetLowerValue(),
                           tetraVisWidget.tetraZCutplaneSlider.GetUpperValue());
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
  // hide all
  cgalTetrahedralizeWidget.hide();
  sofaTetraStuffingWidget.hide();
  genericIsosurfaceStuffingWidget.hide();
  tetgenWidget.hide();
  gmshWidget.hide();
  ftetwildWidget.hide();
  // show only selected
  switch (newSelectedMethod) {
    case 0:
      sofaTetraStuffingWidget.show();
      break;
    case 1:
      cgalTetrahedralizeWidget.show();
      break;
    case 2:
      genericIsosurfaceStuffingWidget.show();
      break;
    case 3:
      tetgenWidget.show();
      break;
    case 4:
      gmshWidget.show();
      break;
    case 5:
      ftetwildWidget.show();
      break;
    default:
      break;
  }
}

void QTetraMesherMainWindow::suggestMethodParameters() {
  const float maxBBox = viewer->getMaxBBox();
  sofaTetraStuffingWidget.tetraSizeSpinBox.setValue(maxBBox * 0.05f);
  cgalTetrahedralizeWidget.cellSizeSpinBox.setValue(maxBBox * 0.1);
  cgalTetrahedralizeWidget.facetSizeSpinBox.setValue(maxBBox * 0.05);
  genericIsosurfaceStuffingWidget.cellSizeSpinBox.setValue(maxBBox * 0.05f);
  gmshWidget.meshSizeMaxSpinBox.setValue(maxBBox * 0.1);
}

void QTetraMesherMainWindow::updateWindowTitle() {
  const QString fileName = viewer->loadedFileName();
  if (fileName.isEmpty())
    setWindowTitle("QTetraMesher");
  else
    setWindowTitle(QFileInfo(fileName).fileName() + " - QTetraMesher");
}

void QTetraMesherMainWindow::loadSurfaceSlot() {
  if (!viewer->loadSurface()) return;
  suggestMethodParameters();
  updateWindowTitle();
}

void QTetraMesherMainWindow::exportSurfaceSlot() { viewer->saveSurface(); }

void QTetraMesherMainWindow::exportGMSHSlot() { viewer->saveGMSH(); }

void QTetraMesherMainWindow::loadTetgenSlot() {
  if (!viewer->loadTetgen()) return;
  suggestMethodParameters();
  updateCutplaneSliders();
  updateWindowTitle();
}

void QTetraMesherMainWindow::exportTetgenSlot() { viewer->saveTetgen(); }

void QTetraMesherMainWindow::loadGMSHSlot() {
  if (tetraJob.isRunning()) return;
  const QString fileName = viewer->selectGMSHFile();
  if (fileName.isEmpty()) return;
  std::cout << "Loading GMSH Mesh... " << fileName.toStdString() << std::endl;
  QString error;
  if (!tetraJob.startLoadGMSH(fileName, &error)) {
    QMessageBox::warning(this, "Load GMSH",
                         QString("Could not load\n%1\n\n%2")
                             .arg(fileName, error));
    return;
  }
  loadingGMSHFile = fileName;
  viewer->beginTask("tetra",
                    QString("Loading %1").arg(QFileInfo(fileName).fileName()),
                    "Press 'Cancel Loading' to stop");
  updateBusyState();
}

void QTetraMesherMainWindow::gmshLoadFinished(
    std::unique_ptr<TetraJob::Result> r, const QString &fileName) {
  if (!r) return;
  if (r->canceled) {
    viewer->displayMessage("Loading the GMSH mesh canceled.", 5000);
    return;
  }
  if (!r->ok) {
    std::cerr << "Loading GMSH failed: " << r->error.toStdString()
              << std::endl;
    viewer->ShowStatusMessage("Loading the GMSH mesh failed...", 10000);
    QMessageBox mb(this);
    mb.setIcon(QMessageBox::Warning);
    mb.setWindowTitle("Load GMSH");
    mb.setText(QString("Could not load a tetrahedral mesh from\n%1\n\n"
                       "The file must be a valid MSH file containing linear "
                       "(4-node) tetrahedra.")
                   .arg(fileName));
    mb.setInformativeText(r->error);
    mb.exec();
    return;
  }
  const size_t numTetras = r->topology->GetTetrahedra().size();
  const size_t numVertices = r->topology->GetVertices().size();
  viewer->tMesh->SetLoadedTetraMesh(std::move(r->topology),
                                    std::move(r->surface));
  viewer->showLoadedMesh(fileName,
                         QString("GMSH mesh loaded in %1 ms. Tetras: %2; "
                                 "Vertices: %3")
                             .arg(r->elapsedMs)
                             .arg(numTetras)
                             .arg(numVertices));
  suggestMethodParameters();
  updateCutplaneSliders();
  updateWindowTitle();
}

void QTetraMesherMainWindow::updateCutplaneSliders() {
  tetraVisWidget.tetraXCutplaneSlider.setLowerValue(0);
  tetraVisWidget.tetraXCutplaneSlider.setUpperValue(100);
  tetraVisWidget.tetraYCutplaneSlider.setLowerValue(0);
  tetraVisWidget.tetraYCutplaneSlider.setUpperValue(100);
  tetraVisWidget.tetraZCutplaneSlider.setLowerValue(0);
  tetraVisWidget.tetraZCutplaneSlider.setUpperValue(100);
  viewer->tMesh->SetXRange(0, 100);
  viewer->tMesh->SetYRange(0, 100);
  viewer->tMesh->SetZRange(0, 100);
}

void QTetraMesherMainWindow::generateSofaTetraStuffingSlot() {
  startOrCancelTetraJob(sofaTetraStuffingWidget);
}

void QTetraMesherMainWindow::generateCGALTetrahedralizeSlot() {
  startOrCancelTetraJob(cgalTetrahedralizeWidget);
}

void QTetraMesherMainWindow::generateGenericIsosurfaceStuffingSlot() {
  startOrCancelTetraJob(genericIsosurfaceStuffingWidget);
}

void QTetraMesherMainWindow::generateTetgenSlot() {
  startOrCancelTetraJob(tetgenWidget);
}

void QTetraMesherMainWindow::generateGMSHSlot() {
  startOrCancelTetraJob(gmshWidget);
}

void QTetraMesherMainWindow::generateFTetWildSlot() {
  startOrCancelTetraJob(ftetwildWidget);
}

void QTetraMesherMainWindow::generateOctreeSlot() {
  if (octreeWatcher.isRunning()) return;
  TetraTools::TriangleTopology *surf = viewer->tMesh->GetSurface();
  if (surf == NULL) {
    QMessageBox::information(this, "Generate Octree",
                             "No surface mesh loaded.");
    return;
  }
  const unsigned int depth =
      (unsigned int)octreeVisWidget.octreeDepthSpinBox.value();
  // loading is disabled while the octree is built, so the data stays valid
  std::vector<Vec3f> *verts = &surf->GetVertices();
  std::vector<Triangle> *tris = &surf->GetTriangles();
  octreeSurface = surf;
  octreePending = true;
  const std::atomic<bool> *stop = &stopThreads;
  viewer->beginTask("octree",
                    QString("Generating octree (depth %1)").arg(depth));
  octreeWatcher.setFuture(
      QtConcurrent::run([verts, tris, depth, stop]() -> Octree * {
        try {
          Octree *octree = new Octree(depth, verts, tris, stop);
          if (stop->load()) {  // incomplete
            delete octree;
            return nullptr;
          }
          return octree;
        } catch (const std::exception &e) {
          std::cerr << "Octree generation failed: " << e.what() << std::endl;
          return nullptr;
        }
      }));
  updateBusyState();
}

void QTetraMesherMainWindow::octreeFinished() {
  viewer->endTask("octree");
  octreePending = false;
  Octree *octree = octreeWatcher.result();
  // the surface cannot change while the octree is built, but be safe
  if (octree != nullptr && viewer->tMesh->GetSurface() != octreeSurface) {
    delete octree;
    octree = nullptr;
  }
  octreeSurface = nullptr;
  updateBusyState();
  if (octree == nullptr) {
    viewer->displayMessage("Octree generation failed...", 5000);
    return;
  }
  viewer->tMesh->SetOctree(octree);
  viewer->update();
  viewer->displayMessage(QString("Octree with depth %1 generated.")
                             .arg(octree->getDepth()),
                         5000);
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

void QTetraMesherMainWindow::closeEvent(QCloseEvent *event) {
  saveSettings();
  stopBackgroundTasks();
  QMainWindow::closeEvent(event);
}

std::vector<TetraMethodWidget *> QTetraMesherMainWindow::tetraMethodWidgets() {
  return {&sofaTetraStuffingWidget, &cgalTetrahedralizeWidget,
          &genericIsosurfaceStuffingWidget, &tetgenWidget, &gmshWidget,
          &ftetwildWidget};
}

void QTetraMesherMainWindow::startOrCancelTetraJob(TetraMethodWidget &widget) {
  // while a job runs, the generate button is a cancel button
  if (tetraJob.isRunning()) {
    tetraJob.cancel();
    return;
  }
  if (viewer->tMesh == NULL || viewer->tMesh->GetSurface() == NULL) {
    QMessageBox::information(this, widget.methodName(),
                             "Please load a surface mesh first.");
    return;
  }
  TetraTools::TriangleTopology *surf = viewer->tMesh->GetSurface();
  QString error;
  if (!tetraJob.start(widget.methodId(), widget.methodName(),
                      widget.parameters(), surf->GetVertices(),
                      surf->GetTriangles(), &error)) {
    QMessageBox mb(this);
    mb.setIcon(QMessageBox::Warning);
    mb.setText(widget.errorHint());
    mb.setInformativeText(error);
    mb.exec();
    return;
  }
  runningTetraWidget = &widget;
  viewer->beginTask("tetra",
                    QString("Generating tetrahedra (%1)").arg(widget.methodName()),
                    "Press 'Cancel Generation' to stop");
  updateBusyState();
}

void QTetraMesherMainWindow::tetraJobFinished() {
  viewer->endTask("tetra");
  TetraMethodWidget *widget = runningTetraWidget;
  runningTetraWidget = nullptr;
  std::unique_ptr<TetraJob::Result> r = tetraJob.takeResult();
  const QString loadedGMSHFile = loadingGMSHFile;
  loadingGMSHFile.clear();
  updateBusyState();
  if (!loadedGMSHFile.isEmpty()) {
    gmshLoadFinished(std::move(r), loadedGMSHFile);
    return;
  }
  if (!r) return;
  if (r->canceled) {
    viewer->displayMessage("Tetrahedralization canceled.", 5000);
    return;
  }
  if (!r->ok) {
    std::cerr << "Tetrahedralization failed: " << r->error.toStdString()
              << std::endl;
    QMessageBox mb(this);
    mb.setIcon(QMessageBox::Warning);
    mb.setText(widget ? widget->errorHint()
                      : QString("ERROR while generating the tetra mesh."));
    mb.setInformativeText(r->error);
    mb.exec();
    return;
  }
  if (widget) widget->applyUsedParameters(r->usedParameters);
  const size_t numTetras = r->topology->GetTetrahedra().size();
  const size_t numVertices = r->topology->GetVertices().size();
  viewer->tMesh->SetTetraTopology(std::move(r->topology));
  updateCutplaneSliders();
  viewer->update();
  viewer->displayMessage(
      QString("Generated tetrahedral mesh (%1) in %2 ms. Tetras: %3; "
              "Vertices: %4")
          .arg(widget ? widget->methodName() : QString())
          .arg(r->elapsedMs)
          .arg(numTetras)
          .arg(numVertices),
      10000);
}

void QTetraMesherMainWindow::updateBusyState() {
  const bool tetraRunning = tetraJob.isRunning();
  const bool octreeRunning = octreeWatcher.isRunning();
  const bool checkRunning = surfaceCheckWatcher.isRunning();
  const bool busy = tetraRunning || octreeRunning || checkRunning;
  // a GMSH load replaces the surface mesh when it is done
  const bool loading = !loadingGMSHFile.isEmpty();
  // loading would replace the surface mesh used by the background tasks
  actionLoadSurface.setEnabled(!busy);
  actionLoadGMSH.setEnabled(!busy);
  actionLoadTetgen.setEnabled(!busy);
  // keep the widget with the cancel button visible while a job runs
  tetraMeshMethodComboBox.setEnabled(!tetraRunning);
  for (TetraMethodWidget *w : tetraMethodWidgets())
    w->setRunning(tetraRunning,
                  loading ? "Cancel Loading" : "Cancel Generation");
  octreeVisWidget.octreeGenerateButton.setEnabled(!octreeRunning && !loading);
  surfaceVisWidget.checkSurfaceButton.setEnabled(!checkRunning && !loading);
}

void QTetraMesherMainWindow::stopBackgroundTasks() {
  // kills the worker process; its result is discarded
  tetraJob.cancel();
  // the octree and the surface check stop at the next check of the flag
  stopThreads = true;
  octreeWatcher.waitForFinished();
  surfaceCheckWatcher.waitForFinished();
  if (octreePending) {
    // finished, but the result was never handed over to the mesh
    octreePending = false;
    disconnect(&octreeWatcher, nullptr, this, nullptr);
    delete octreeWatcher.result();
  }
}

void QTetraMesherMainWindow::on_viewer_onLoad() {}

// Visualization options are stored with QSettings (an ini file on Linux, the
// registry on Windows). Colors are stored as #rrggbb strings.
void QTetraMesherMainWindow::updateColorButtons() {
  auto style = [](const QColor &c) {
    return QString("QPushButton{background-color: rgb(%1,%2,%3);}\n")
        .arg(c.red())
        .arg(c.green())
        .arg(c.blue());
  };
  surfaceVisWidget.surfaceColorButton.setStyleSheet(
      style(viewer->tMesh->GetSurfaceColor()));
  surfaceVisWidget.surfaceWireframeColorButton.setStyleSheet(
      style(viewer->tMesh->GetSurfaceWireframeColor()));
  tetraVisWidget.tetraColorButton.setStyleSheet(
      style(viewer->tMesh->GetTetraColor()));
  tetraVisWidget.tetraOuterColorButton.setStyleSheet(
      style(viewer->tMesh->GetTetraOuterColor()));
  tetraVisWidget.tetraInnerColorButton.setStyleSheet(
      style(viewer->tMesh->GetTetraInnerColor()));
  tetraVisWidget.tetraWireframeColorButton.setStyleSheet(
      style(viewer->tMesh->GetTetraWireframeColor()));
}

void QTetraMesherMainWindow::loadSettings() {
  QSettings settings;
  settings.beginGroup("visualization");
  auto color = [&settings](const QString &key, const QColor &fallback) {
    const QColor c(settings.value(key, fallback.name()).toString());
    return c.isValid() ? c : fallback;
  };
  QGLTetraMesh *mesh = viewer->tMesh;
  QColor c = color("surfaceColor", mesh->GetSurfaceColor());
  mesh->SetSurfaceSolidColor(c.red(), c.green(), c.blue());
  c = color("surfaceWireframeColor", mesh->GetSurfaceWireframeColor());
  mesh->SetSurfaceWireframeColor(c.red(), c.green(), c.blue());
  c = color("tetraColor", mesh->GetTetraColor());
  mesh->SetTetraSolidColor(c.red(), c.green(), c.blue());
  c = color("tetraOuterColor", mesh->GetTetraOuterColor());
  mesh->SetTetraOuterColor(c.red(), c.green(), c.blue());
  c = color("tetraInnerColor", mesh->GetTetraInnerColor());
  mesh->SetTetraInnerColor(c.red(), c.green(), c.blue());
  c = color("tetraWireframeColor", mesh->GetTetraWireframeColor());
  mesh->SetTetraWireframeColor(c.red(), c.green(), c.blue());
  updateColorButtons();

  // the widgets' signals apply these to the viewer
  tetraVisWidget.drawWireframeCheckbox.setChecked(
      settings.value("tetraWireframe", true).toBool());
  tetraVisWidget.drawSolidCheckbox.setChecked(
      settings.value("tetraSolid", true).toBool());
  tetraVisWidget.depthColoringCheckbox.setChecked(
      settings.value("tetraDepthColoring", true).toBool());
  const int surfaceMode = settings.value("surfaceMode", 0).toInt();
  if (surfaceMode >= 0 &&
      surfaceMode < surfaceVisWidget.surfaceVisComboBox.count())
    surfaceVisWidget.surfaceVisComboBox.setCurrentIndex(surfaceMode);
  actionShowAxis.setChecked(settings.value("showAxis", true).toBool());
  actionShowGrid.setChecked(settings.value("showGrid", true).toBool());
  actionShowBBox.setChecked(settings.value("showBBox", false).toBool());
  settings.endGroup();
  repairOnLoadCheckBox.setChecked(
      settings.value("loading/repairOnLoad", true).toBool());
  viewer->update();

  // restoreGeometry() keeps the window on a connected screen
  if (!restoreGeometry(settings.value("window/geometry").toByteArray()))
    resize(1400, 900);
}

void QTetraMesherMainWindow::saveSettings() {
  QSettings settings;
  settings.setValue("window/geometry", saveGeometry());
  settings.beginGroup("visualization");
  settings.setValue("surfaceColor", viewer->tMesh->GetSurfaceColor().name());
  settings.setValue("surfaceWireframeColor",
                    viewer->tMesh->GetSurfaceWireframeColor().name());
  settings.setValue("tetraColor", viewer->tMesh->GetTetraColor().name());
  settings.setValue("tetraOuterColor",
                    viewer->tMesh->GetTetraOuterColor().name());
  settings.setValue("tetraInnerColor",
                    viewer->tMesh->GetTetraInnerColor().name());
  settings.setValue("tetraDepthColoring",
                    tetraVisWidget.depthColoringCheckbox.isChecked());
  settings.setValue("tetraWireframeColor",
                    viewer->tMesh->GetTetraWireframeColor().name());
  settings.setValue("tetraWireframe",
                    tetraVisWidget.drawWireframeCheckbox.isChecked());
  settings.setValue("tetraSolid", tetraVisWidget.drawSolidCheckbox.isChecked());
  settings.setValue("surfaceMode",
                    surfaceVisWidget.surfaceVisComboBox.currentIndex());
  settings.setValue("showAxis", actionShowAxis.isChecked());
  settings.setValue("showGrid", actionShowGrid.isChecked());
  settings.setValue("showBBox", actionShowBBox.isChecked());
  settings.endGroup();
  settings.setValue("loading/repairOnLoad", repairOnLoadCheckBox.isChecked());
}
