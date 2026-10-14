/*
 * GMSHWidget.cpp
 *
 * This widget provides options for the GMSH tetrahedralization method.
 *
 *  Created 2024
 *      Author: Dennis Luebke
 */

#include <QElapsedTimer>
#include <QFont>
#include <QMessageBox>
#include <GMSHWidget.hpp>
#include <sstream>

#include "GMSHWrapper.h"

GMSHWidget::GMSHWidget(QWidget *parent) : BaseOptionsWidget(parent) {
  labelTitle.setText("GMSH Tetrahedralize");
  QFont boldFont;
  boldFont.setBold(true);
  labelTitle.setFont(boldFont);

  meshSizeMinLabel.setText("Mesh Size Min (0=auto)");
  meshSizeMinLabel.setFont(boldFont);
  meshSizeMinSpinBox.setValue(0.0);
  meshSizeMinSpinBox.setSingleStep(0.01);
  meshSizeMinSpinBox.setMinimum(0.0);
  meshSizeMinSpinBox.setDecimals(4);
  meshSizeMinLabel.setMaximumHeight(20);
  meshSizeMinSpinBox.setMaximumHeight(20);
  meshSizeMinLayout.addWidget(&meshSizeMinLabel);
  meshSizeMinLayout.addWidget(&meshSizeMinSpinBox);

  meshSizeMaxLabel.setText("Mesh Size Max (0=auto)");
  meshSizeMaxLabel.setFont(boldFont);
  meshSizeMaxSpinBox.setValue(0.0);
  meshSizeMaxSpinBox.setSingleStep(0.01);
  meshSizeMaxSpinBox.setMinimum(0.0);
  meshSizeMaxSpinBox.setDecimals(4);
  meshSizeMaxLabel.setMaximumHeight(20);
  meshSizeMaxSpinBox.setMaximumHeight(20);
  meshSizeMaxLayout.addWidget(&meshSizeMaxLabel);
  meshSizeMaxLayout.addWidget(&meshSizeMaxSpinBox);

  algorithmLabel.setText("Algorithm");
  algorithmLabel.setFont(boldFont);
  algorithmComboBox.addItem("Delaunay (1)");
  algorithmComboBox.addItem("Frontal (4)");
  algorithmComboBox.addItem("HXT (10)");
  algorithmLabel.setMaximumHeight(20);
  algorithmComboBox.setMaximumHeight(24);
  algorithmLayout.addWidget(&algorithmLabel);
  algorithmLayout.addWidget(&algorithmComboBox);

  generateTetrahedraButton.setText("Generate Tetrahedra");

  layout.addWidget(&labelTitle);
  layout.addLayout(&meshSizeMinLayout);
  layout.addLayout(&meshSizeMaxLayout);
  layout.addLayout(&algorithmLayout);
  layout.addWidget(&generateTetrahedraButton);
  layout.addStretch();
}

void GMSHWidget::generateTetrahedra(QGLTetraViewer *viewer_) {
  QElapsedTimer t;
  t.start();
  QGLTetraMesh *tMesh_ = viewer_->tMesh;
  if (tMesh_ == nullptr) {
    return;
  }
  const std::vector<Triangle> &tris =
      tMesh_->GetTriangleTopology()->GetTriangles();
  const std::vector<Vec3f> &verts =
      tMesh_->GetTriangleTopology()->GetVertices();
  if (tMesh_->GetSurface() == nullptr) {
    return;
  }

  // Map combo-box index to GMSH algorithm number
  static const int algorithmMap[] = {1, 4, 10};
  const int algoIdx = algorithmComboBox.currentIndex();
  const int algorithm3D = algorithmMap[algoIdx];

  GMSHWrapper *gw = new GMSHWrapper();
  try {
    gw->GenerateFromSurface(tris, verts,
                            meshSizeMinSpinBox.value(),
                            meshSizeMaxSpinBox.value(),
                            algorithm3D);
    tMesh_->UpdateTetraMesh(gw->GetTetraVertices(), gw->GetTetras());
    viewer_->update();
    std::stringstream ss;
    ss << "Generated GMSH tetrahedral mesh in " << t.elapsed()
       << "ms. Tetras: " << gw->GetTetras().size()
       << "; Vertices: " << gw->GetTetraVertices().size();
    const std::string stdMSG = ss.str();
    viewer_->displayMessage(QString::fromStdString(stdMSG), 10000);
    delete gw;
    gw = nullptr;
  } catch (std::exception &e) {
    std::cerr << "Caught exception in GMSHWrapper..." << std::endl;
    if (gw != nullptr) {
      delete gw;
    }
    QMessageBox mb(this);
    mb.setIcon(QMessageBox::Warning);
    mb.setText(
        "ERROR while generating GMSH tetra mesh.<br />"
        "Check the surface mesh and parameters.");
    mb.setInformativeText(e.what());
    mb.exec();
  }
}
