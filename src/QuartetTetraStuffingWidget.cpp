/*
 * QuartetTetraStuffingWidget.cpp
 *
 * This widget provides options for Crawford Doran's implementation Quartet
 * TetraStuffing implementation.
 *
 *  Created 2024
 *      Author: Dennis Luebke
 */

#include <QuartetTetraStuffing.h>

#include <QElapsedTimer>
#include <QFont>
#include <QMessageBox>
#include <QuartetTetraStuffingWidget.hpp>
#include <sstream>

QuartetTetraStuffingWidget::QuartetTetraStuffingWidget(QWidget *parent)
    : BaseOptionsWidget(parent) {
  labelTitle.setText("Quartet TetraStuffing");
  QFont boldFont;
  boldFont.setBold(true);
  labelTitle.setFont(boldFont);
  cellSizeLabel.setText("CellSize");
  cellSizeLabel.setFont(boldFont);
  cellSizeSpinBox.setValue(1.0);
  cellSizeSpinBox.setSingleStep(0.01);
  cellSizeLabel.setMaximumHeight(20);
  cellSizeSpinBox.setMaximumHeight(20);
  tetraSizeLayout.addWidget(&cellSizeLabel);
  tetraSizeLayout.addWidget(&cellSizeSpinBox);
  angleThresholdLabel.setText("Angle Threshold");
  angleThresholdSpinBox.setValue(15.0);
  angleThresholdSpinBox.setSingleStep(0.05);
  angleThresholdLabel.setMaximumHeight(20);
  angleThresholdSpinBox.setMaximumHeight(20);
  angleThresholdLayout.addWidget(&angleThresholdLabel);
  angleThresholdLayout.addWidget(&angleThresholdSpinBox);
  generateTetrahedraButton.setText("Generate Tetrahedra");
  layout.addWidget(&labelTitle);
  layout.addLayout(&tetraSizeLayout);
  layout.addLayout(&angleThresholdLayout);
  layout.addWidget(&generateTetrahedraButton);
  layout.addStretch();
}

void QuartetTetraStuffingWidget::generateTetrahedra(QGLTetraViewer *viewer_) {
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
  std::vector<inTriangle> inTris;
  for (const auto &tri : tris) {
    inTriangle t;
    t.index[0] = tri.index[0];
    t.index[1] = tri.index[1];
    t.index[2] = tri.index[2];
    inTris.push_back(t);
  }
  std::vector<tmpVec3f> inVerts;
  for (const auto &vert : verts) {
    tmpVec3f v;
    v.x = vert.x;
    v.y = vert.y;
    v.z = vert.z;
    inVerts.push_back(v);
  }
  QuartetTetraStuffing *cth = new QuartetTetraStuffing();
  try {
    cth->GenerateFromSurface(inTris, inVerts, float(cellSizeSpinBox.value()),
                             float(angleThresholdSpinBox.value()), true);
    std::vector<Vec3f> outVerts;
    for (const auto &vert : cth->GetTetraVertices()) {
      Vec3f v;
      v.x = vert.x;
      v.y = vert.y;
      v.z = vert.z;
      outVerts.push_back(v);
    }
    std::vector<Tetrahedron> outTetras;
    for (const auto &tet : cth->GetTetras()) {
      Tetrahedron t;
      t.index[0] = tet.index[0];
      t.index[1] = tet.index[1];
      t.index[2] = tet.index[2];
      t.index[3] = tet.index[3];
      outTetras.push_back(t);
    }
    tMesh_->UpdateTetraMesh(outVerts, outTetras);
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
