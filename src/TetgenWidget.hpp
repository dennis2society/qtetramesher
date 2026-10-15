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

#include <TetraMethodWidget.hpp>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QPushButton>

class TetgenWidget : public TetraMethodWidget {
  Q_OBJECT

 public:
  TetgenWidget(QWidget *parent);
  void update();

  QString methodId() const override { return "tetgen"; }
  QString methodName() const override { return "Tetgen"; }
  QVariantMap parameters() const override;
  void applyUsedParameters(const QVariantMap &used) override;
  QString errorHint() const override {
    return "ERROR while generating Tetgen tetra mesh...<br />Check the surface mesh and parameters.";
  }
  QPushButton &generateButton() override { return generateTetrahedraButton; }

  QHBoxLayout qualityBoundsLayout;
  QLabel qualityBoundsLabel;
  QDoubleSpinBox qualityBoundsSpinBox;

  QHBoxLayout cellSizeLayout;
  QLabel cellSizeLabel;
  QDoubleSpinBox cellSizeSpinBox;

  QCheckBox usePLCCheckBox;

  QPushButton generateTetrahedraButton;
};

#endif  // TETGEN_WIDGET_HPP
