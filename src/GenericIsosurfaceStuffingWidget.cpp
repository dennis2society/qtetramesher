/*
 * GenericIsosurfaceStuffingWidget.cpp
 *
 * This widget provides the UI options for the Generic Isosurface Stuffing
 * implementation (Shewchuk's algorithm).
 *
 *  Created 2026
 */

#include "GenericIsosurfaceStuffingWidget.hpp"

#include <QFont>


GenericIsosurfaceStuffingWidget::GenericIsosurfaceStuffingWidget(QWidget *parent)
    : TetraMethodWidget(parent) {
  labelTitle.setText("Generic Isosurface Stuffing");
  QFont boldFont;
  boldFont.setBold(true);
  labelTitle.setFont(boldFont);
  cellSizeLabel.setText("CellSize");
  cellSizeLabel.setFont(boldFont);
  cellSizeSpinBox.setValue(1.0);
  cellSizeSpinBox.setSingleStep(0.01);
  cellSizeLabel.setMaximumHeight(20);
  cellSizeSpinBox.setMaximumHeight(20);
  cellSizeLayout.addWidget(&cellSizeLabel);
  cellSizeLayout.addWidget(&cellSizeSpinBox);
  alphaShortLabel.setText("AlphaShort");
  // Labelle & Shewchuk's proven thresholds; larger values warp lattice points
  // so far that the stencils degenerate into slivers.
  alphaShortSpinBox.setDecimals(4);
  alphaShortSpinBox.setRange(0.0, 0.49);
  alphaShortSpinBox.setValue(0.3548);
  alphaShortSpinBox.setSingleStep(0.01);
  alphaLongLabel.setText("AlphaLong");
  alphaLongSpinBox.setDecimals(4);
  alphaLongSpinBox.setRange(0.0, 0.49);
  alphaLongSpinBox.setValue(0.2151);
  alphaLongSpinBox.setSingleStep(0.01);
  alphaShortLabel.setMaximumHeight(20);
  alphaShortSpinBox.setMaximumHeight(20);
  alphaLongLabel.setMaximumHeight(20);
  alphaLongSpinBox.setMaximumHeight(20);
  alphaLayout.addWidget(&alphaShortLabel);
  alphaLayout.addWidget(&alphaShortSpinBox);
  alphaLayout.addWidget(&alphaLongLabel);
  alphaLayout.addWidget(&alphaLongSpinBox);

  auto percentText = [](int v) {
    return v == 0 ? QString("Off") : QString("%1%").arg(v);
  };
  gradingLabel.setText("Grading");
  gradingSlider.setOrientation(Qt::Horizontal);
  gradingSlider.setRange(0, 100);
  gradingSlider.setValue(50);
  gradingSlider.setToolTip(
      "0 = uniform lattice. Higher values let the interior tetrahedra grow "
      "larger (up to 128x the cell size) and keep larger tetrahedra along "
      "flat parts of the surface. Curved regions and sharp edges stay at the "
      "cell size.");
  gradingValueLabel.setMinimumWidth(32);
  gradingValueLabel.setText(percentText(gradingSlider.value()));
  connect(&gradingSlider, &QSlider::valueChanged, this,
          [this, percentText](int v) { gradingValueLabel.setText(percentText(v)); });
  gradingLayout.addWidget(&gradingLabel);
  gradingLayout.addWidget(&gradingSlider);
  gradingLayout.addWidget(&gradingValueLabel);

  featureAngleLabel.setText("Sharp Edge Angle");
  featureAngleSpinBox.setRange(0.0, 180.0);
  featureAngleSpinBox.setDecimals(0);
  featureAngleSpinBox.setSuffix(QString::fromUtf8("\u00b0"));
  featureAngleSpinBox.setSpecialValueText("Off");
  featureAngleSpinBox.setValue(30.0);
  featureAngleSpinBox.setToolTip(
      "Surface edges whose faces meet at a larger angle are treated as sharp "
      "edges that the tetra mesh should follow.");
  featureAngleLabel.setMaximumHeight(20);
  featureAngleSpinBox.setMaximumHeight(20);
  featureAngleLayout.addWidget(&featureAngleLabel);
  featureAngleLayout.addWidget(&featureAngleSpinBox);

  featureSnapLabel.setText("Edge Snapping");
  featureSnapSlider.setOrientation(Qt::Horizontal);
  featureSnapSlider.setRange(0, 100);
  featureSnapSlider.setValue(50);
  featureSnapSlider.setToolTip(
      "How far boundary vertices may move onto sharp edges and corners, "
      "relative to the local tetra size. A move is only done if all "
      "tetrahedra around the vertex stay valid.");
  featureSnapValueLabel.setMinimumWidth(32);
  featureSnapValueLabel.setText(percentText(featureSnapSlider.value()));
  connect(&featureSnapSlider, &QSlider::valueChanged, this,
          [this, percentText](int v) {
            featureSnapValueLabel.setText(percentText(v));
          });
  featureSnapLayout.addWidget(&featureSnapLabel);
  featureSnapLayout.addWidget(&featureSnapSlider);
  featureSnapLayout.addWidget(&featureSnapValueLabel);

  generateTetrahedraButton.setText("Generate Tetrahedra");
  layout.addWidget(&labelTitle);
  layout.addLayout(&cellSizeLayout);
  layout.addLayout(&alphaLayout);
  layout.addLayout(&gradingLayout);
  layout.addLayout(&featureAngleLayout);
  layout.addLayout(&featureSnapLayout);
  layout.addWidget(&generateTetrahedraButton);
}

QVariantMap GenericIsosurfaceStuffingWidget::parameters() const {
  QVariantMap p;
  p["cellSize"] = cellSizeSpinBox.value();
  p["alphaShort"] = alphaShortSpinBox.value();
  p["alphaLong"] = alphaLongSpinBox.value();
  p["grading"] = gradingSlider.value() / 100.0;
  p["featureAngle"] = featureAngleSpinBox.value();
  p["featureSnap"] = featureSnapSlider.value() / 100.0;
  return p;
}
