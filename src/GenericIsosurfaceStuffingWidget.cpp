/*
 * GenericIsosurfaceStuffingWidget.cpp
 *
 * This widget provides the UI options for the Generic Isosurface Stuffing
 * implementation (Shewchuk's algorithm).
 *
 *  Created 2026
 */

#include <stdexcept>
#include "GenericIsosurfaceStuffingWidget.hpp"

#include <QElapsedTimer>
#include <QFont>
#include <QMessageBox>
#include <sstream>

#include "GenericIsosurfaceStuffing.h"

GenericIsosurfaceStuffingWidget::GenericIsosurfaceStuffingWidget(QWidget *parent)
    : BaseOptionsWidget(parent) {
  labelTitle.setText("Generic Isosurface Stuffing");
  QFont boldFont;
  boldFont.setBold(true);
  labelTitle.setFont(boldFont);
  cellSizeLabel.setText("CellSize");
  cellSizeLabel.setFont(boldFont);
  cellSizeSpinBox.setValue(1.0);
  cellSizeSpinBox.setSingleStep(0.01);
  cellSizeLabel.setMaximumHeight(20);
  cellSizeSpinBox.setMaximumHeight(20);
  cellSizeLayout.addWidget(&cellSizeLabel);
  cellSizeLayout.addWidget(&cellSizeSpinBox);
  alphaShortLabel.setText("AlphaShort");
  // Labelle & Shewchuk's proven thresholds; larger values warp lattice points
  // so far that the stencils degenerate into slivers.
  alphaShortSpinBox.setDecimals(4);
  alphaShortSpinBox.setRange(0.0, 0.49);
  alphaShortSpinBox.setValue(0.3548);
  alphaShortSpinBox.setSingleStep(0.01);
  alphaLongLabel.setText("AlphaLong");
  alphaLongSpinBox.setDecimals(4);
  alphaLongSpinBox.setRange(0.0, 0.49);
  alphaLongSpinBox.setValue(0.2151);
  alphaLongSpinBox.setSingleStep(0.01);
  alphaShortLabel.setMaximumHeight(20);
  alphaShortSpinBox.setMaximumHeight(20);
  alphaLongLabel.setMaximumHeight(20);
  alphaLongSpinBox.setMaximumHeight(20);
  alphaLayout.addWidget(&alphaShortLabel);
  alphaLayout.addWidget(&alphaShortSpinBox);
  alphaLayout.addWidget(&alphaLongLabel);
  alphaLayout.addWidget(&alphaLongSpinBox);
  gradedCheckBox.setText("Graded (large interior tets)");
  gradedCheckBox.setChecked(false);
  generateTetrahedraButton.setText("Generate Tetrahedra");
  layout.addWidget(&labelTitle);
  layout.addLayout(&cellSizeLayout);
  layout.addLayout(&alphaLayout);
  layout.addWidget(&gradedCheckBox);
  layout.addWidget(&generateTetrahedraButton);
  layout.addStretch();
}

void GenericIsosurfaceStuffingWidget::generateTetrahedra(QGLTetraViewer *viewer_) {
  QElapsedTimer t;
  t.start();
  if (viewer_ == NULL || viewer_->tMesh == NULL) {
    return;
  }
  QGLTetraMesh *tMesh_ = viewer_->tMesh;
  // check this first, the topology getters below dereference the surface
  if (tMesh_->GetSurface() == NULL) {
    QMessageBox::information(this, "Generic Isosurface Stuffing",
                             "Please load a surface mesh first.");
    return;
  }
  const std::vector<Triangle> &tris =
      tMesh_->GetTriangleTopology()->GetTriangles();
  const std::vector<Vec3f> &verts =
      tMesh_->GetTriangleTopology()->GetVertices();
  try {
    GenericIsosurfaceStuffing w;
    w.GenerateFromSurface(tris, verts, float(cellSizeSpinBox.value()),
                          float(alphaShortSpinBox.value()),
                          float(alphaLongSpinBox.value()),
                          gradedCheckBox.isChecked());
    if (!tMesh_->UpdateTetraMesh(w.GetTetraVertices(),
                                 w.GetTetras())) {
      throw std::runtime_error(
          "The method did not generate a valid tetrahedral mesh.");
    }
    viewer_->update();
    std::stringstream ss;
    ss << "Generated tetrahedral mesh in " << t.elapsed()
       << "ms. Tetras: " << w.GetTetras().size()
       << "; Vertices: " << w.GetTetraVertices().size();
    viewer_->displayMessage(QString::fromStdString(ss.str()), 10000);
  } catch (const std::exception &e) {
    std::cerr << "Caught exception in GenericIsosurfaceStuffing: " << e.what() << std::endl;
    viewer_->update();
    QMessageBox mb(this);
    mb.setIcon(QMessageBox::Warning);
    mb.setText(
        "ERROR while generating Generic Isosurface Stuffing tetra mesh.");
    mb.setInformativeText(e.what());
    mb.exec();
  } catch (...) {
    std::cerr << "Caught unknown exception in GenericIsosurfaceStuffing..." << std::endl;
    viewer_->update();
    QMessageBox mb(this);
    mb.setIcon(QMessageBox::Warning);
    mb.setText(
        "ERROR while generating Generic Isosurface Stuffing tetra mesh.");
    mb.setInformativeText("Unknown error.");
    mb.exec();
  }
}