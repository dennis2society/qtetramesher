/**
 * BaseTetraDialog.cpp
 *
 *  Created 2012
 *      Author: Dennis Luebke
 */

#include "BaseTetraDialog.h"
#include <QTime>
#include <sstream>

BaseTetraDialog::BaseTetraDialog(QWidget *parent) : QDialog(parent) {
  _viewer = NULL;
}

void BaseTetraDialog::setViewer(QGLTetraViewer *viewer_) { _viewer = viewer_; }

BaseTetraDialog::~BaseTetraDialog() {}
