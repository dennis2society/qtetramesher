/*
 * GMSHWidget.hpp
 *
 * This widget provides options for the GMSH tetrahedralization method.
 *
 *  Created 2024
 *      Author: Dennis Luebke
 */
#ifndef GMSH_WIDGET_HPP
#define GMSH_WIDGET_HPP

#include <qgltetraviewer.hpp>

#include <BaseOptionsWidget.hpp>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QPushButton>

class GMSHWidget : public BaseOptionsWidget {
  Q_OBJECT

 public:
  GMSHWidget(QWidget *parent);
  void update();

  void generateTetrahedra(QGLTetraViewer *viewer_);

  QHBoxLayout meshSizeMinLayout;
  QLabel meshSizeMinLabel;
  QDoubleSpinBox meshSizeMinSpinBox;

  QHBoxLayout meshSizeMaxLayout;
  QLabel meshSizeMaxLabel;
  QDoubleSpinBox meshSizeMaxSpinBox;

  QHBoxLayout algorithmLayout;
  QLabel algorithmLabel;
  QComboBox algorithmComboBox;

  QPushButton generateTetrahedraButton;
};

#endif  // GMSH_WIDGET_HPP
