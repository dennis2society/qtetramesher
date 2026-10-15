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

#include <TetraMethodWidget.hpp>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QPushButton>

class GMSHWidget : public TetraMethodWidget {
  Q_OBJECT

 public:
  GMSHWidget(QWidget *parent);
  void update();

  QString methodId() const override { return "gmsh"; }
  QString methodName() const override { return "GMSH"; }
  QVariantMap parameters() const override;
  void applyUsedParameters(const QVariantMap &used) override;
  QString errorHint() const override {
    return "ERROR while generating GMSH tetra mesh.<br />Check the surface mesh and parameters.";
  }
  QPushButton &generateButton() override { return generateTetrahedraButton; }

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
