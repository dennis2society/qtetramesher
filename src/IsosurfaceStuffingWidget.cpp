/*
 * IsosurfaceStuffingWidget.cpp
 *
 * This widget provides the UI options for the Isosurface Stuffing
 * implementation (Shewchuk's algorithm).
 *
 *  Created 2026
 */

#include "IsosurfaceStuffingWidget.hpp"

#include <QElapsedTimer>
#include <QFont>
#include <QMessageBox>
#include <sstream>

#include "IsosurfaceStuffing.h"

IsosurfaceStuffingWidget::IsosurfaceStuffingWidget(QWidget *parent)
    : BaseOptionsWidget(parent) {
  labelTitle.setText("Isosurface Stuffing");
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
  alphaShortSpinBox.setValue(0.4);
  alphaShortSpinBox.setSingleStep(0.01);
  alphaLongLabel.setText("AlphaLong");
  alphaLongSpinBox.setValue(0.27);
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

void IsosurfaceStuffingWidget::generateTetrahedra(QGLTetraViewer *viewer_) {
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
  IsosurfaceStuffing iso;
  try {
    iso.GenerateFromSurface(tris, verts,
                            float(cellSizeSpinBox.value()),
                            float(alphaShortSpinBox.value()),
                            float(alphaLongSpinBox.value()),
                            gradedCheckBox.isChecked());
    tMesh_->UpdateTetraMesh(iso.GetTetraVertices(), iso.GetTetras());
    tMesh_->Draw();
    viewer_->update();
    std::stringstream ss;
    ss << "Generated tetrahedral mesh in " << t.elapsed()
       << "ms. Tetras: " << iso.GetTetras().size()
       << "; Vertices: " << iso.GetTetraVertices().size();
    std::string stdMSG = ss.str();
    viewer_->displayMessage(QString::fromStdString(stdMSG), 10000);
  } catch (std::exception &e) {
    std::cerr << "Caught exception in IsosurfaceStuffing..." << std::endl;
    QMessageBox mb(this);
    mb.setIcon(QMessageBox::Warning);
    mb.setText(
        "ERROR while generating Isosurface Stuffing tetra mesh.");
    mb.setInformativeText(e.what());
    mb.exec();
  }
}
