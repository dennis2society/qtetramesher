/*
 * TetgenWidget.hpp
 *
 * This widget provides options for the Tetgen method.
 *
 *  Created 2024
 *      Author: Dennis Luebke
 */
#ifndef TETGEN_WIDGET_HPP
#define TETGEN_WIDGET_HPP

#include <qgltetraviewer.h>

#include <BaseOptionsWidget.hpp>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QPushButton>

class TetgenWidget : public BaseOptionsWidget {
  Q_OBJECT

 public:
  TetgenWidget(QWidget *parent);
  void update();

  void generateTetrahedra(QGLTetraViewer *viewer_);

  QLabel notAvailebleLabel;
};

#endif  // TETGEN_WIDGET_HPP
