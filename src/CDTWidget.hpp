/*
 * CDTWidget.hpp
 *
 * This widget provides options for the constrained Delaunay tetrahedrization
 * (CDT) method. https://github.com/MarcoAttene/CDT
 */
#ifndef CDT_WIDGET_HPP
#define CDT_WIDGET_HPP

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <TetraMethodWidget.hpp>

class CDTWidget : public TetraMethodWidget {
  Q_OBJECT

 public:
  CDTWidget(QWidget *parent);

  QString methodId() const override { return "cdt"; }
  QString methodName() const override { return "CDT"; }
  QVariantMap parameters() const override;
  QString errorHint() const override {
    return "ERROR while generating CDT tetra mesh.<br />Check the surface "
           "mesh (e.g. degenerate triangles) and parameters.";
  }
  QPushButton &generateButton() override { return generateTetrahedraButton; }

  QLabel infoLabel;

  QHBoxLayout interiorSpacingLayout;
  QLabel interiorSpacingLabel;
  QDoubleSpinBox interiorSpacingSpinBox;

  QCheckBox innerOnlyCheckBox;
  QCheckBox boundingBoxCheckBox;
  QCheckBox verboseCheckBox;

  QPushButton generateTetrahedraButton;
};

#endif  // CDT_WIDGET_HPP
