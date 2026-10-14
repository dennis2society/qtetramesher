/*
 * CGALTetrahedralizeWidget.cpp
 *
 * This widget provides options for the CGAL delaunay triangulation.
 *
 *  Created 2024
 *      Author: Dennis Luebke
 */

#include <stdexcept>
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
  facetAngleSpinBox.setMaximumHeight(20);
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
  facetDistanceSpinBox.setMaximumHeight(20);
  cellRadiusToEdgeRatioSpinBox.setValue(3.0);
  cellRadiusToEdgeRatioSpinBox.setSingleStep(0.01);
  cellRadiusToEdgeRatioSpinBox.setMaximumHeight(20);
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
  if (viewer_ == NULL || viewer_->tMesh == NULL) {
    return;
  }
  QGLTetraMesh *tMesh_ = viewer_->tMesh;
  // check this first, the topology getters below dereference the surface
  if (tMesh_->GetSurface() == NULL) {
    QMessageBox::information(this, "CGAL Tetrahedralize",
                             "Please load a surface mesh first.");
    return;
  }
  const std::vector<Triangle> &tris =
      tMesh_->GetTriangleTopology()->GetTriangles();
  const std::vector<Vec3f> &verts =
      tMesh_->GetTriangleTopology()->GetVertices();
  try {
    CGALTetrahedralize w;
    w.GenerateFromSurface(
        tris, verts, cellSizeSpinBox.value(), facetAngleSpinBox.value(),
        facetSizeSpinBox.value(), facetDistanceSpinBox.value(),
        cellRadiusToEdgeRatioSpinBox.value());
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
    std::cerr << "Caught exception in CGALTetrahedralize: " << e.what() << std::endl;
    viewer_->update();
    QMessageBox mb(this);
    mb.setIcon(QMessageBox::Warning);
    mb.setText(
        "ERROR while generating CGAL tetra mesh...<br />Probably the surface mesh is violating a CGAL precondition.");
    mb.setInformativeText(e.what());
    mb.exec();
  } catch (...) {
    std::cerr << "Caught unknown exception in CGALTetrahedralize..." << std::endl;
    viewer_->update();
    QMessageBox mb(this);
    mb.setIcon(QMessageBox::Warning);
    mb.setText(
        "ERROR while generating CGAL tetra mesh...<br />Probably the surface mesh is violating a CGAL precondition.");
    mb.setInformativeText("Unknown error.");
    mb.exec();
  }
}