/*
 * TetgenWidget.cpp
 *
 * This widget provides options for the Tetgen method.
 *
 *  Created 2024
 *      Author: Dennis Luebke
 */

#include <stdexcept>
#include <QElapsedTimer>
#include <QFont>
#include <QMessageBox>
#include <TetgenWidget.hpp>
#include <sstream>

#include "TetgenWrapper.h"

TetgenWidget::TetgenWidget(QWidget *parent) : BaseOptionsWidget(parent) {
  labelTitle.setText("Tetgen");
  QFont boldFont;
  boldFont.setBold(true);
  labelTitle.setFont(boldFont);

  qualityBoundsLabel.setText("Quality Bound");
  qualityBoundsLabel.setFont(boldFont);
  qualityBoundsSpinBox.setValue(2.0);
  qualityBoundsSpinBox.setSingleStep(0.1);
  qualityBoundsSpinBox.setMinimum(1.0);
  qualityBoundsLabel.setMaximumHeight(20);
  qualityBoundsSpinBox.setMaximumHeight(20);
  qualityBoundsLayout.addWidget(&qualityBoundsLabel);
  qualityBoundsLayout.addWidget(&qualityBoundsSpinBox);

  cellSizeLabel.setText("Cell Size (0=auto)");
  cellSizeLabel.setFont(boldFont);
  cellSizeSpinBox.setValue(0.0);
  cellSizeSpinBox.setDecimals(4);
  cellSizeSpinBox.setMaximum(1000000.0);
  cellSizeSpinBox.setSingleStep(0.01);
  cellSizeSpinBox.setMinimum(0.0);
  cellSizeLabel.setMaximumHeight(20);
  cellSizeSpinBox.setMaximumHeight(20);
  cellSizeLayout.addWidget(&cellSizeLabel);
  cellSizeLayout.addWidget(&cellSizeSpinBox);

  usePLCCheckBox.setText("Use PLC");
  usePLCCheckBox.setChecked(true);

  generateTetrahedraButton.setText("Generate Tetrahedra");

  layout.addWidget(&labelTitle);
  layout.addLayout(&qualityBoundsLayout);
  layout.addLayout(&cellSizeLayout);
  layout.addWidget(&usePLCCheckBox);
  layout.addWidget(&generateTetrahedraButton);
  layout.addStretch();
}

void TetgenWidget::generateTetrahedra(QGLTetraViewer *viewer_) {
  QElapsedTimer t;
  t.start();
  if (viewer_ == NULL || viewer_->tMesh == NULL) {
    return;
  }
  QGLTetraMesh *tMesh_ = viewer_->tMesh;
  // check this first, the topology getters below dereference the surface
  if (tMesh_->GetSurface() == NULL) {
    QMessageBox::information(this, "Tetgen",
                             "Please load a surface mesh first.");
    return;
  }
  const std::vector<Triangle> &tris =
      tMesh_->GetTriangleTopology()->GetTriangles();
  const std::vector<Vec3f> &verts =
      tMesh_->GetTriangleTopology()->GetVertices();
  try {
    TetgenWrapper w;
    w.GenerateFromSurface(tris, verts,
                          static_cast<float>(qualityBoundsSpinBox.value()),
                          static_cast<float>(cellSizeSpinBox.value()),
                          usePLCCheckBox.isChecked());
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
    std::cerr << "Caught exception in TetgenWrapper: " << e.what() << std::endl;
    viewer_->update();
    QMessageBox mb(this);
    mb.setIcon(QMessageBox::Warning);
    mb.setText(
        "ERROR while generating Tetgen tetra mesh...<br />Check the surface mesh and parameters.");
    mb.setInformativeText(e.what());
    mb.exec();
  } catch (...) {
    std::cerr << "Caught unknown exception in TetgenWrapper..." << std::endl;
    viewer_->update();
    QMessageBox mb(this);
    mb.setIcon(QMessageBox::Warning);
    mb.setText(
        "ERROR while generating Tetgen tetra mesh...<br />Check the surface mesh and parameters.");
    mb.setInformativeText("Unknown error.");
    mb.exec();
  }
}