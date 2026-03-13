/*
 * TetgenWidget.cpp
 *
 * This widget provides options for the Tetgen method.
 *
 *  Created 2024
 *      Author: Dennis Luebke
 */

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
  QGLTetraMesh *tMesh_ = viewer_->tMesh;
  if (tMesh_ == NULL) {
    return;
  }
  const std::vector<Triangle> &tris =
      tMesh_->GetTriangleTopology()->GetTriangles();
  const std::vector<Vec3f> &verts =
      tMesh_->GetTriangleTopology()->GetVertices();
  if (tMesh_->GetSurface() == NULL) {
    return;
  }
  TetgenWrapper *tw = new TetgenWrapper();
  try {
    tw->GenerateFromSurface(tris, verts,
                            static_cast<float>(qualityBoundsSpinBox.value()),
                            static_cast<float>(cellSizeSpinBox.value()),
                            usePLCCheckBox.isChecked());
    tMesh_->UpdateTetraMesh(tw->GetTetraVertices(), tw->GetTetras());
    tMesh_->Draw();
    viewer_->update();
    std::stringstream ss;
    ss << "Generated tetrahedral mesh in " << t.elapsed()
       << "ms. Tetras: " << tw->GetTetras().size()
       << "; Vertices: " << tw->GetTetraVertices().size();
    std::string stdMSG = ss.str();
    viewer_->displayMessage(QString::fromStdString(stdMSG), 10000);
    delete tw;
    tw = NULL;
  } catch (std::exception &e) {
    std::cerr << "Caught exception in TetgenWrapper..." << std::endl;
    if (tw != NULL) {
      delete tw;
    }
    QMessageBox mb(this);
    mb.setIcon(QMessageBox::Warning);
    mb.setText(
        "ERROR while generating Tetgen tetra mesh...<br />Check the surface "
        "mesh and parameters.");
    mb.setInformativeText(e.what());
    mb.exec();
  }
}
