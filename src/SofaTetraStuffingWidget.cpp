/*
 * SofaTetraStuffingWidget.cpp
 *
 * This widget provides the UI options for the SOFA TetraStuffing
 * implementation.
 *
 *  Created 2024
 *      Author: Dennis Luebke
 */

#include "SofaTetraStuffingWidget.hpp"
#include "SofaTetraStuffing.h"
#include <QElapsedTimer>
#include <QFont>
#include <sstream>

SofaTetraStuffingWidget::SofaTetraStuffingWidget(QWidget *parent)
    : BaseOptionsWidget(parent) {
  labelTitle.setText("SOFA TetraStuffing");
  QFont boldFont;
  boldFont.setBold(true);
  labelTitle.setFont(boldFont);
  tetraSizeLabel.setText("TetraSize");
  tetraSizeLabel.setFont(boldFont);
  tetraSizeSpinBox.setValue(2.0);
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
  SofaTetraStuffing sts;
  sts.GenerateFromSurface(tris, verts, tetraSizeSpinBox.value(),
                          alphaShortSpinBox.value(), alphaLongSpinBox.value(),
                          snapToPointsCheckBox.isChecked(),
                          splitTetrasCheckBox.isChecked());
  tMesh_->UpdateTetraMesh(sts.GetTetraVertices(), sts.GetTetras());
  tMesh_->Draw();
  viewer_->update();
  std::cout << "Generated tetrahedral mesh in " << t.elapsed()
            << " ms.\nTetras: " << sts.GetTetras().size()
            << "; Vertices: " << sts.GetTetraVertices().size() << std::endl;
}
