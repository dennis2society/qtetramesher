/*
 * FTetWildWidget.hpp
 *
 * This widget provides options for the optional fTetWild tetrahedralization
 * method. https://github.com/wildmeshing/fTetWild
 */
#ifndef FTETWILD_WIDGET_HPP
#define FTETWILD_WIDGET_HPP

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <TetraMethodWidget.hpp>

class FTetWildWidget : public TetraMethodWidget {
  Q_OBJECT

 public:
  FTetWildWidget(QWidget *parent);

  QString methodId() const override { return "ftetwild"; }
  QString methodName() const override { return "fTetWild"; }
  QVariantMap parameters() const override;
  QString errorHint() const override {
    return "ERROR while generating fTetWild tetra mesh.<br />Check the "
           "surface mesh and parameters.";
  }
  QPushButton &generateButton() override { return generateTetrahedraButton; }

  QHBoxLayout edgeLengthLayout;
  QLabel edgeLengthLabel;
  QDoubleSpinBox edgeLengthSpinBox;

  QHBoxLayout epsilonLayout;
  QLabel epsilonLabel;
  QDoubleSpinBox epsilonSpinBox;

  QHBoxLayout stopEnergyLayout;
  QLabel stopEnergyLabel;
  QDoubleSpinBox stopEnergySpinBox;

  QCheckBox coarsenCheckBox;

  QPushButton generateTetrahedraButton;
};

#endif  // FTETWILD_WIDGET_HPP
