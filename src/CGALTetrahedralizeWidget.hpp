/*
 * CGALTetrahedralizeWidget.hpp
 *
 * This widget provides options for the CGAL delaunay triangulation.
 *
 *  Created 2024
 *      Author: Dennis Luebke
 */
#ifndef CGAL_TETRAHEDRALIZE_WIDGET_HPP
#define CGAL_TETRAHEDRALIZE_WIDGET_HPP

#include <qgltetraviewer.hpp>

#include <BaseOptionsWidget.hpp>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QPushButton>

class CGALTetrahedralizeWidget : public BaseOptionsWidget {
  Q_OBJECT

 public:
  CGALTetrahedralizeWidget(QWidget *parent);
  void update();

  void generateTetrahedra(QGLTetraViewer *viewer_);

  QHBoxLayout cellSizeLayout;
  QLabel cellSizeLabel;
  QDoubleSpinBox cellSizeSpinBox;
  QHBoxLayout facetAngleLayout;
  QLabel facetAngleLabel;
  QDoubleSpinBox facetAngleSpinBox;
  QLabel facetSizeLabel;
  QDoubleSpinBox facetSizeSpinBox;
  QHBoxLayout facetDistanceLayout;
  QLabel facetDistanceLabel;
  QDoubleSpinBox facetDistanceSpinBox;
  QLabel cellRadiusToEdgeRatioLabel;
  QDoubleSpinBox cellRadiusToEdgeRatioSpinBox;
  QPushButton generateTetrahedraButton;
};

#endif  // CGAL_TETRAHEDRALIZE_WIDGET_HPP
