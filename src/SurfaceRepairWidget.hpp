/*
 * SurfaceRepairWidget.hpp
 *
 * Options for repairing (and optionally remeshing) the loaded surface mesh
 * with geogram.
 */

#ifndef SURFACE_REPAIR_WIDGET_H
#define SURFACE_REPAIR_WIDGET_H

#include <BaseOptionsWidget.hpp>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QPushButton>
#include <QSpinBox>
#include <TetraMeshTools/MeshRepair.h>

class SurfaceRepairWidget : public BaseOptionsWidget {
  Q_OBJECT

 public:
  SurfaceRepairWidget(QWidget *parent);

  GeogramRepairOptions options() const;
  void setOptions(const GeogramRepairOptions &options_);
  /// default for the remeshing, e.g. the vertex count of a loaded surface
  void setRemeshVertices(std::size_t n_);

  QPushButton repairButton;

 private:
  QGridLayout grid;
  QCheckBox removeSmallPartsCheckBox;
  QDoubleSpinBox minPartAreaSpinBox;  // percent of the total area
  QCheckBox fillHolesCheckBox;
  QCheckBox resolveIntersectionsCheckBox;
  QCheckBox remeshCheckBox;
  QSpinBox remeshVerticesSpinBox;
};

#endif  // SURFACE_REPAIR_WIDGET_H
