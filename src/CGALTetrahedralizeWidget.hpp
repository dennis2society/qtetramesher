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

#include <TetraMethodWidget.hpp>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QPushButton>

class CGALTetrahedralizeWidget : public TetraMethodWidget {
  Q_OBJECT

 public:
  CGALTetrahedralizeWidget(QWidget *parent);
  void update();

  QString methodId() const override { return "cgal"; }
  QString methodName() const override { return "CGAL Delaunay Triangulation"; }
  QVariantMap parameters() const override;
  QString errorHint() const override {
    return "ERROR while generating CGAL tetra mesh...<br />Probably the surface mesh is violating a CGAL precondition.";
  }
  QPushButton &generateButton() override { return generateTetrahedraButton; }

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
