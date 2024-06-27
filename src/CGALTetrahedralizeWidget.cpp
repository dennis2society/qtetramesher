/*
 * CGALTetrahedralizeWidget.cpp
 *
 * This widget provides options for the CGAL delaunay triangulation.
 *
 *  Created 2024
 *      Author: Dennis Luebke
 */

#include "CGALTetrahedralizeWidget.hpp"

#include <QElapsedTimer>
#include <QFont>
#include <QMessageBox>
#include <sstream>

#include "CGALTetrahedralize.h"

CGALTetrahedralizeWidget::CGALTetrahedralizeWidget(QWidget *parent)
    : BaseOptionsWidget(parent) {
  labelTitle.setText("CGAL Delaunay Triangulation");
  QFont boldFont;
  boldFont.setBold(true);
  labelTitle.setFont(boldFont);
  cellSizeLabel.setText("CellSize");
  cellSizeLabel.setFont(boldFont);
  cellSizeSpinBox.setValue(0.5);
  cellSizeSpinBox.setSingleStep(0.01);
  cellSizeLabel.setMaximumHeight(20);
  cellSizeSpinBox.setMaximumHeight(20);
  cellSizeLayout.addWidget(&cellSizeLabel);
  cellSizeLayout.addWidget(&cellSizeSpinBox);
  facetAngleLabel.setText("Facet Angle");
  facetAngleSpinBox.setValue(25.0);
  facetAngleSpinBox.setSingleStep(0.05);
  facetSizeLabel.setText("Facet Size");
  facetSizeSpinBox.setValue(0.50);
  facetSizeSpinBox.setSingleStep(0.01);
  facetSizeLabel.setMaximumHeight(20);
  facetSizeSpinBox.setMaximumHeight(20);
  facetSizeLabel.setMaximumHeight(20);
  facetSizeSpinBox.setMaximumHeight(20);
  facetAngleLayout.addWidget(&facetAngleLabel);
  facetAngleLayout.addWidget(&facetAngleSpinBox);
  facetAngleLayout.addWidget(&facetSizeLabel);
  facetAngleLayout.addWidget(&facetSizeSpinBox);
  facetDistanceLabel.setText("Facet Distance");
  cellRadiusToEdgeRatioLabel.setText("Cell2Edge Ratio");
  facetDistanceLabel.setMaximumHeight(20);
  cellRadiusToEdgeRatioLabel.setMaximumHeight(20);
  facetDistanceSpinBox.setValue(0.03);
  facetDistanceSpinBox.setSingleStep(0.01);
  cellRadiusToEdgeRatioSpinBox.setValue(3.0);
  cellRadiusToEdgeRatioSpinBox.setSingleStep(0.01);
  facetDistanceLayout.addWidget(&facetDistanceLabel);
  facetDistanceLayout.addWidget(&facetDistanceSpinBox);
  facetDistanceLayout.addWidget(&cellRadiusToEdgeRatioLabel);
  facetDistanceLayout.addWidget(&cellRadiusToEdgeRatioSpinBox);
  generateTetrahedraButton.setText("Generate Tetrahedra");
  layout.addWidget(&labelTitle);
  layout.addLayout(&cellSizeLayout);
  layout.addLayout(&facetAngleLayout);
  layout.addLayout(&facetDistanceLayout);
  layout.addWidget(&generateTetrahedraButton);
  layout.addStretch();
}

void CGALTetrahedralizeWidget::generateTetrahedra(QGLTetraViewer *viewer_) {
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
  CGALTetrahedralize *cth = new CGALTetrahedralize();
  try {
    cth->GenerateFromSurface(
        tris, verts, cellSizeSpinBox.value(), facetAngleSpinBox.value(),
        facetSizeSpinBox.value(), facetDistanceSpinBox.value(),
        cellRadiusToEdgeRatioSpinBox.value());
    tMesh_->UpdateTetraMesh(cth->GetTetraVertices(), cth->GetTetras());
    tMesh_->Draw();
    viewer_->update();
    std::stringstream ss;
    ss << "Generated tetrahedral mesh in " << t.elapsed()
       << "ms. Tetras: " << cth->GetTetras().size()
       << "; Vertices: " << cth->GetTetraVertices().size();
    std::string stdMSG = ss.str();
    delete cth;
    cth = NULL;
  } catch (std::exception e) {
    std::cerr << "Caught exception in CGALTetrahedralize..." << std::endl;
    if (cth != NULL) {
      delete cth;
    }
    QMessageBox mb(this);
    mb.setIcon(QMessageBox::Warning);
    mb.setText(
        "ERROR while generating CGAL tetra mesh...<br />Probably the "
        "surface mesh is violating a CGAL precondition.");
    mb.setInformativeText(e.what());
    mb.exec();
  }
}
