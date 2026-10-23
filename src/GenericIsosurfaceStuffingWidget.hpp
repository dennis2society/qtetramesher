/*
 * GenericIsosurfaceStuffingWidget.hpp
 *
 * This widget provides options for the Generic Isosurface Stuffing implementation
 * (Shewchuk's algorithm, self-contained C++ implementation).
 *
 *  Created 2026
 */
#ifndef GENERIC_ISOSURFACE_STUFFING_WIDGET_HPP
#define GENERIC_ISOSURFACE_STUFFING_WIDGET_HPP

#include <TetraMethodWidget.hpp>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QSlider>

class GenericIsosurfaceStuffingWidget : public TetraMethodWidget {
  Q_OBJECT

 public:
  GenericIsosurfaceStuffingWidget(QWidget *parent);
  void update();

  QString methodId() const override { return "generic"; }
  QString methodName() const override { return "Generic Isosurface Stuffing"; }
  QVariantMap parameters() const override;
  QString errorHint() const override {
    return "ERROR while generating Generic Isosurface Stuffing tetra mesh.";
  }
  QPushButton &generateButton() override { return generateTetrahedraButton; }

  QHBoxLayout cellSizeLayout;
  QLabel cellSizeLabel;
  QDoubleSpinBox cellSizeSpinBox;
  QHBoxLayout alphaLayout;
  QLabel alphaShortLabel;
  QDoubleSpinBox alphaShortSpinBox;
  QLabel alphaLongLabel;
  QDoubleSpinBox alphaLongSpinBox;
  QHBoxLayout gradingLayout;
  QLabel gradingLabel;
  QSlider gradingSlider;
  QLabel gradingValueLabel;
  QHBoxLayout featureAngleLayout;
  QLabel featureAngleLabel;
  QDoubleSpinBox featureAngleSpinBox;
  QHBoxLayout featureSnapLayout;
  QLabel featureSnapLabel;
  QSlider featureSnapSlider;
  QLabel featureSnapValueLabel;
  QPushButton generateTetrahedraButton;
};

#endif  // GENERIC_ISOSURFACE_STUFFING_WIDGET_HPP
