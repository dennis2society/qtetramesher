/*
 * SofaTetraStuffingWidget.hpp
 *
 * This widget provides options for the SOFA TetraStuffing implementation.
 *
 *  Created 2024
 *      Author: Dennis Luebke
 */
#ifndef SOFA_TETRA_STUFFING_WIDGET_HPP
#define SOFA_TETRA_STUFFING_WIDGET_HPP

#include <TetraMethodWidget.hpp>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QPushButton>

class SofaTetraStuffingWidget : public TetraMethodWidget {
  Q_OBJECT

 public:
  SofaTetraStuffingWidget(QWidget *parent);
  void update();

  QString methodId() const override { return "sofa"; }
  QString methodName() const override { return "SOFA TetraStuffing"; }
  QVariantMap parameters() const override;
  QString errorHint() const override {
    return "ERROR while generating SOFA tetra mesh.";
  }
  QPushButton &generateButton() override { return generateTetrahedraButton; }

  QHBoxLayout tetraSizeLayout;
  QLabel tetraSizeLabel;
  QDoubleSpinBox tetraSizeSpinBox;
  QHBoxLayout alphaOptionsLayout;
  QLabel alphaShortLabel;
  QDoubleSpinBox alphaShortSpinBox;
  QLabel alphaLongLabel;
  QDoubleSpinBox alphaLongSpinBox;
  QCheckBox splitTetrasCheckBox;
  QCheckBox snapToPointsCheckBox;
  QPushButton generateTetrahedraButton;
  QHBoxLayout additionalOptionsLayout;
};

#endif  // SOFA_TETRA_STUFFING_WIDGET_HPP
