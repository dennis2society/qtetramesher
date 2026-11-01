/*
 * CDTWidget.cpp
 *
 * This widget provides options for the constrained Delaunay tetrahedrization
 * (CDT) method.
 */

#include <CDTWidget.hpp>
#include <QFont>

CDTWidget::CDTWidget(QWidget *parent) : TetraMethodWidget(parent) {
  labelTitle.setText("CDT (Constrained Delaunay)");
  QFont boldFont;
  boldFont.setBold(true);
  labelTitle.setFont(boldFont);

  infoLabel.setText(
      "Keeps the surface triangles exactly, using exact arithmetic. "
      "There is no quality refinement.");
  infoLabel.setWordWrap(true);

  interiorSpacingLabel.setText("Interior Spacing (0=none)");
  interiorSpacingLabel.setFont(boldFont);
  interiorSpacingLabel.setToolTip(
      "Adds a regular grid of points with this spacing inside the bounding "
      "box (keeping half the spacing away from the surface). Without them, "
      "the mesh only has the surface vertices and few Steiner points.");
  interiorSpacingSpinBox.setRange(0.0, 1000000.0);
  interiorSpacingSpinBox.setDecimals(4);
  interiorSpacingSpinBox.setSingleStep(0.01);
  interiorSpacingSpinBox.setValue(0.0);
  interiorSpacingLabel.setMaximumHeight(20);
  interiorSpacingSpinBox.setMaximumHeight(20);
  interiorSpacingLayout.addWidget(&interiorSpacingLabel);
  interiorSpacingLayout.addWidget(&interiorSpacingSpinBox);

  innerOnlyCheckBox.setText("Remove outer tetrahedra");
  innerOnlyCheckBox.setToolTip(
      "Keep only the tetrahedra inside the surface. If the surface is not "
      "closed, the whole convex hull is kept anyway.");
  innerOnlyCheckBox.setChecked(true);
  innerOnlyCheckBox.setMaximumHeight(20);

  boundingBoxCheckBox.setText("Add bounding box vertices");
  boundingBoxCheckBox.setToolTip(
      "Encloses the surface in a slightly enlarged box with 8 extra vertices "
      "before tetrahedrizing (CDT option -b)");
  boundingBoxCheckBox.setMaximumHeight(20);

  verboseCheckBox.setText("Verbose output");
  verboseCheckBox.setToolTip("Print the progress of the algorithm");
  verboseCheckBox.setMaximumHeight(20);

  generateTetrahedraButton.setText("Generate Tetrahedra");

  layout.addWidget(&labelTitle);
  layout.addWidget(&infoLabel);
  layout.addLayout(&interiorSpacingLayout);
  layout.addWidget(&innerOnlyCheckBox);
  layout.addWidget(&boundingBoxCheckBox);
  layout.addWidget(&verboseCheckBox);
  layout.addWidget(&generateTetrahedraButton);
}

QVariantMap CDTWidget::parameters() const {
  QVariantMap p;
  p["interiorSpacing"] = interiorSpacingSpinBox.value();
  p["innerOnly"] = innerOnlyCheckBox.isChecked();
  p["boundingBox"] = boundingBoxCheckBox.isChecked();
  p["verbose"] = verboseCheckBox.isChecked();
  return p;
}
