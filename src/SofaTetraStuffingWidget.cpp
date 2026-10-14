/*
 * SofaTetraStuffingWidget.cpp
 *
 * This widget provides the UI options for the SOFA TetraStuffing
 * implementation.
 *
 *  Created 2024
 *      Author: Dennis Luebke
 */

#include <QMessageBox>
#include <stdexcept>
#include "SofaTetraStuffingWidget.hpp"

#include <QElapsedTimer>
#include <QFont>
#include <sstream>

#include "SofaTetraStuffing.h"

SofaTetraStuffingWidget::SofaTetraStuffingWidget(QWidget *parent)
    : BaseOptionsWidget(parent) {
  labelTitle.setText("SOFA TetraStuffing");
  QFont boldFont;
  boldFont.setBold(true);
  labelTitle.setFont(boldFont);
  tetraSizeLabel.setText("TetraSize");
  tetraSizeLabel.setFont(boldFont);
  tetraSizeSpinBox.setValue(1.0);
  tetraSizeSpinBox.setSingleStep(0.01);
  tetraSizeLabel.setMaximumHeight(20);
  tetraSizeSpinBox.setMaximumHeight(20);
  tetraSizeLayout.addWidget(&tetraSizeLabel);
  tetraSizeLayout.addWidget(&tetraSizeSpinBox);
  alphaShortLabel.setText("AlphaShort");
  alphaShortSpinBox.setValue(0.43);
  alphaShortSpinBox.setSingleStep(0.01);
  alphaLongLabel.setText("AlphaLong");
  alphaLongSpinBox.setValue(0.25);
  alphaLongSpinBox.setSingleStep(0.01);
  alphaShortLabel.setMaximumHeight(20);
  alphaShortSpinBox.setMaximumHeight(20);
  alphaLongLabel.setMaximumHeight(20);
  alphaLongSpinBox.setMaximumHeight(20);
  alphaOptionsLayout.addWidget(&alphaShortLabel);
  alphaOptionsLayout.addWidget(&alphaShortSpinBox);
  alphaOptionsLayout.addWidget(&alphaLongLabel);
  alphaOptionsLayout.addWidget(&alphaLongSpinBox);
  splitTetrasCheckBox.setText("Split Tetras");
  splitTetrasCheckBox.setChecked(true);
  snapToPointsCheckBox.setText("Snap To Points");
  snapToPointsCheckBox.setChecked(true);
  additionalOptionsLayout.addWidget(&splitTetrasCheckBox);
  additionalOptionsLayout.addWidget(&snapToPointsCheckBox);
  generateTetrahedraButton.setText("Generate Tetrahedra");
  layout.addWidget(&labelTitle);
  layout.addLayout(&tetraSizeLayout);
  layout.addLayout(&alphaOptionsLayout);
  layout.addLayout(&additionalOptionsLayout);
  layout.addWidget(&generateTetrahedraButton);
  layout.addStretch();
}

void SofaTetraStuffingWidget::generateTetrahedra(QGLTetraViewer *viewer_) {
  QElapsedTimer t;
  t.start();
  if (viewer_ == NULL || viewer_->tMesh == NULL) {
    return;
  }
  QGLTetraMesh *tMesh_ = viewer_->tMesh;
  // check this first, the topology getters below dereference the surface
  if (tMesh_->GetSurface() == NULL) {
    QMessageBox::information(this, "SOFA TetraStuffing",
                             "Please load a surface mesh first.");
    return;
  }
  const std::vector<Triangle> &tris =
      tMesh_->GetTriangleTopology()->GetTriangles();
  const std::vector<Vec3f> &verts =
      tMesh_->GetTriangleTopology()->GetVertices();
  try {
    SofaTetraStuffing w;
    w.GenerateFromSurface(tris, verts, tetraSizeSpinBox.value(),
                          alphaShortSpinBox.value(), alphaLongSpinBox.value(),
                          snapToPointsCheckBox.isChecked(),
                          splitTetrasCheckBox.isChecked());
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
    std::cerr << "Caught exception in SofaTetraStuffing: " << e.what() << std::endl;
    viewer_->update();
    QMessageBox mb(this);
    mb.setIcon(QMessageBox::Warning);
    mb.setText(
        "ERROR while generating SOFA tetra mesh.");
    mb.setInformativeText(e.what());
    mb.exec();
  } catch (...) {
    std::cerr << "Caught unknown exception in SofaTetraStuffing..." << std::endl;
    viewer_->update();
    QMessageBox mb(this);
    mb.setIcon(QMessageBox::Warning);
    mb.setText(
        "ERROR while generating SOFA tetra mesh.");
    mb.setInformativeText("Unknown error.");
    mb.exec();
  }
}