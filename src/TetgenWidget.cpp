/*
 * TetgenWidget.cpp
 *
 * This widget provides options for the Tetgen method.
 *
 *  Created 2024
 *      Author: Dennis Luebke
 */

#include <QElapsedTimer>
#include <QFont>
#include <QMessageBox>
#include <TetgenWidget.hpp>
#include <sstream>

TetgenWidget::TetgenWidget(QWidget *parent) : BaseOptionsWidget(parent) {
  labelTitle.setText("Tetgen");
  QFont boldFont;
  boldFont.setBold(true);
  notAvailebleLabel.setFont(boldFont);
  notAvailebleLabel.setText("Düdümm... \nDisabled... Sorry!");
  layout.addWidget(&labelTitle);
  layout.addWidget(&notAvailebleLabel);
  layout.addStretch();
}

void TetgenWidget::generateTetrahedra(QGLTetraViewer *viewer_) {}
