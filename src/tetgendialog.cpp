/*
 * cgaltetrahedralizedialog.h
 *
 *  Created 2012
 *      Author: Dennis Luebke
 */

// #include "QuartetTetraStuffing.h"
#include <QElapsedTimer>
#include <QMessageBox>
#include <TetgenWrapper.h>
#include <sstream>
#include <tetgendialog.h>
#include <ui_tetgendialog.h>

TetgenDialog::TetgenDialog(QWidget *parent)
    : BaseTetraDialog(parent), m_ui(new Ui::TetgenDialog) {
  m_ui->setupUi(this);
  _viewer = NULL;
}

TetgenDialog::~TetgenDialog() { delete m_ui; }

void TetgenDialog::setParams(const float volumeConstraint,
                             const float qualityBound, bool usePLC) {
  m_ui->volumeConstraintSpinBox->setValue(volumeConstraint);
  m_ui->qualityBoundSpinBox->setValue(qualityBound);
  m_ui->usePlcCheckbox->setChecked(usePLC);
}

void TetgenDialog::generateTetras() {
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
  TetgenWrapper *ttw = new TetgenWrapper;
  try {
    emit displayMessage(QString("Generating tetra mesh..."), 1000);
    ttw->GenerateFromSurface(tris, verts, m_ui->qualityBoundSpinBox->value(),
                             m_ui->volumeConstraintSpinBox->value(),
                             m_ui->usePlcCheckbox->isChecked());
    emit displayMessage(QString("Updating visual mesh..."), 2000);
    tMesh_->UpdateTetraMesh(ttw->GetTetraVertices(), ttw->GetTetras());
    std::stringstream ss;
    ss << "Generated tetrahedral mesh in " << t.elapsed()
       << "ms. Tetras: " << ttw->GetTetras().size()
       << "; Vertices: " << ttw->GetTetraVertices().size();
    std::string stdMSG = ss.str();
    emit notifyDone(QString::fromStdString(stdMSG), 10000);
    delete ttw;
    ttw = nullptr;
  } catch (std::exception e) {
    std::cerr << "Caught exception in Tetgen Tetrahedralize......" << std::endl;
    if (ttw != nullptr) {
      delete ttw;
      ttw = nullptr;
    }
    QMessageBox mb(this);
    mb.setIcon(QMessageBox::Warning);
    mb.setText("ERROR while generating Tetgen tetra mesh...<br />Probably the "
               "surface mesh is violating a Tetgen precondition.");
    mb.setInformativeText(e.what());
    mb.exec();
  }
}
