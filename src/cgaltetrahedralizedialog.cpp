/*
 * cgaltetrahedralizedialog.h
 *
 *  Created 2012
 *      Author: Dennis Luebke
 */

#include "cgaltetrahedralizedialog.h"
#include "CGALTetrahedralize.h"
#include "ui_CGALTetrahedralizeDialog.h"
#include <QElapsedTimer>
#include <QMessageBox>
#include <sstream>

CGALTetrahedralizeDialog::CGALTetrahedralizeDialog(QWidget *parent)
    : BaseTetraDialog(parent), m_ui(new Ui::CGALTetrahedralizeDialog) {
  m_ui->setupUi(this);
  _viewer = NULL;
}

CGALTetrahedralizeDialog::~CGALTetrahedralizeDialog() { delete m_ui; }

void CGALTetrahedralizeDialog::setParams(const float tetraSize_,
                                         const float faceSize_) {
  m_ui->cellSizeSpinBox->setValue(tetraSize_);
  m_ui->facetSizeSpinBox->setValue(faceSize_);
}

void CGALTetrahedralizeDialog::generateTetras() {
  QElapsedTimer t;
  t.start();
  QGLTetraMesh *tMesh_ = _viewer->tMesh;
  if (tMesh_ == NULL) {
    emit displayMessage(QString("ERROR! No surface mesh loaded..."), 5000);
    return;
  }
  const std::vector<Triangle> &tris =
      tMesh_->GetTriangleTopology()->GetTriangles();
  const std::vector<Vec3f> &verts =
      tMesh_->GetTriangleTopology()->GetVertices();
  if (tMesh_->GetSurface() == NULL) {
    emit displayMessage(QString("ERROR! No surface mesh loaded..."), 5000);
    return;
  }
  CGALTetrahedralize *cth = new CGALTetrahedralize();
  try {
    emit displayMessage(QString("Generating tetra mesh..."), 1000);
    cth->GenerateFromSurface(
        tris, verts, m_ui->cellSizeSpinBox->value(),
        m_ui->facetAngleSpinBox->value(), m_ui->facetSizeSpinBox->value(),
        m_ui->facetDistanceSpinBox->value(), m_ui->crterSpinBox->value());
    emit displayMessage(QString("Updating visual mesh..."), 2000);
    tMesh_->UpdateTetraMesh(cth->GetTetraVertices(), cth->GetTetras());
    std::stringstream ss;
    ss << "Generated tetrahedral mesh in " << t.elapsed()
       << "ms. Tetras: " << cth->GetTetras().size()
       << "; Vertices: " << cth->GetTetraVertices().size();
    std::string stdMSG = ss.str();
    emit notifyDone(QString::fromStdString(stdMSG), 10000);
    delete cth;
    cth = NULL;
  } catch (std::exception e) {
    std::cerr << "Caught exception in CGALTetrahedralize..." << std::endl;
    if (cth != NULL) {
      delete cth;
    }
    QMessageBox mb(this);
    mb.setIcon(QMessageBox::Warning);
    mb.setText("ERROR while generating CGAL tetra mesh...<br />Probably the "
               "surface mesh is violating a CGAL precondition.");
    mb.setInformativeText(e.what());
    mb.exec();
  }
}
