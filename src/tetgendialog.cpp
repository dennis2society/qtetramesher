/*
 * cgaltetrahedralizedialog.h
 *
 *  Created 2012
 *      Author: Dennis Luebke
 */

//#include "QuartetTetraStuffing.h"
#include "tetgendialog.h"
#include "ui_tetgendialog.h"
#include <QElapsedTimer>
#include <QMessageBox>
#include <sstream>

TetgenDialog::TetgenDialog(QWidget *parent)
    : BaseTetraDialog(parent), m_ui(new Ui::TetgenDialog) {
  m_ui->setupUi(this);
  _viewer = NULL;
}

TetgenDialog::~TetgenDialog() { delete m_ui; }

void TetgenDialog::setParams(const float volumeConstraint,
                             const float qualityBound,
                             bool usePLC) {
  m_ui->volumeConstraintSpinBox->setValue(volumeConstraint);
  m_ui->qualityBoundSpinBox->setValue(qualityBound);
  m_ui->usePlcCheckbox->setChecked(usePLC);
}

void TetgenDialog::generateTetras() {

}
