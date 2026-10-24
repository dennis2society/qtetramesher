/*
 * SurfaceRepairWidget.cpp
 */

#include <QFont>
#include <SurfaceRepairWidget.hpp>
#include <algorithm>

SurfaceRepairWidget::SurfaceRepairWidget(QWidget *parent)
    : BaseOptionsWidget(parent) {
  labelTitle.setText("Surface Repair (geogram)");
  QFont boldFont;
  boldFont.setBold(true);
  labelTitle.setFont(boldFont);
  setToolTip(
      "Repairs the loaded surface mesh so that Tetgen, GMSH and CGAL can "
      "mesh it. Duplicate vertices, degenerate triangles and non-manifold "
      "edges are always fixed. A generated tetra mesh is removed.");

  removeSmallPartsCheckBox.setText("Remove small parts below");
  removeSmallPartsCheckBox.setToolTip(
      "Removes disconnected parts (debris) smaller than this percentage of "
      "the total surface area");
  minPartAreaSpinBox.setRange(0.001, 50.0);
  minPartAreaSpinBox.setDecimals(3);
  minPartAreaSpinBox.setSingleStep(0.1);
  minPartAreaSpinBox.setSuffix(" % area");
  minPartAreaSpinBox.setValue(0.1);
  grid.addWidget(&removeSmallPartsCheckBox, 0, 0);
  grid.addWidget(&minPartAreaSpinBox, 0, 1);

  fillHolesCheckBox.setText("Fill holes");
  fillHolesCheckBox.setToolTip("Closes all holes of the surface");
  grid.addWidget(&fillHolesCheckBox, 1, 0, 1, 2);

  resolveIntersectionsCheckBox.setText(
      "Resolve self-intersections (keep outer hull)");
  resolveIntersectionsCheckBox.setToolTip(
      "Splits intersecting triangles with exact arithmetic and removes all "
      "parts inside the outer hull (also overlapping and internal parts)");
  grid.addWidget(&resolveIntersectionsCheckBox, 2, 0, 1, 2);

  remeshCheckBox.setText("Remesh with");
  remeshCheckBox.setToolTip(
      "Replaces the triangles by evenly shaped ones (better input for Tetgen "
      "and GMSH). Sharp edges are rounded and small details may be lost.");
  remeshVerticesSpinBox.setRange(100, 2000000);
  remeshVerticesSpinBox.setSingleStep(1000);
  remeshVerticesSpinBox.setSuffix(" vertices");
  remeshVerticesSpinBox.setGroupSeparatorShown(true);
  grid.addWidget(&remeshCheckBox, 3, 0);
  grid.addWidget(&remeshVerticesSpinBox, 3, 1);
  connect(&removeSmallPartsCheckBox, &QCheckBox::toggled, &minPartAreaSpinBox,
          &QWidget::setEnabled);
  connect(&remeshCheckBox, &QCheckBox::toggled, &remeshVerticesSpinBox,
          &QWidget::setEnabled);

  setOptions(GeogramRepairOptions());

  repairButton.setText("Repair Surface");
  repairButton.setMaximumHeight(20);
  layout.addWidget(&labelTitle);
  layout.addLayout(&grid);
  layout.addWidget(&repairButton);
}

GeogramRepairOptions SurfaceRepairWidget::options() const {
  GeogramRepairOptions o;
  o.removeSmallParts = removeSmallPartsCheckBox.isChecked();
  o.minPartArea = minPartAreaSpinBox.value() / 100.0;
  o.fillHoles = fillHolesCheckBox.isChecked();
  o.resolveIntersections = resolveIntersectionsCheckBox.isChecked();
  o.remesh = remeshCheckBox.isChecked();
  o.remeshVertices = std::size_t(remeshVerticesSpinBox.value());
  return o;
}

void SurfaceRepairWidget::setOptions(const GeogramRepairOptions &o) {
  removeSmallPartsCheckBox.setChecked(o.removeSmallParts);
  minPartAreaSpinBox.setValue(o.minPartArea * 100.0);
  minPartAreaSpinBox.setEnabled(o.removeSmallParts);
  fillHolesCheckBox.setChecked(o.fillHoles);
  resolveIntersectionsCheckBox.setChecked(o.resolveIntersections);
  remeshCheckBox.setChecked(o.remesh);
  remeshVerticesSpinBox.setEnabled(o.remesh);
  setRemeshVertices(o.remeshVertices);
}

void SurfaceRepairWidget::setRemeshVertices(std::size_t n_) {
  remeshVerticesSpinBox.setValue(int(std::min<std::size_t>(
      n_, std::size_t(remeshVerticesSpinBox.maximum()))));
}
