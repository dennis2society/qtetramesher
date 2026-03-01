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

#include <qgltetraviewer.hpp>

#include <BaseOptionsWidget.hpp>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QPushButton>

class GenericIsosurfaceStuffingWidget : public BaseOptionsWidget {
  Q_OBJECT

 public:
  GenericIsosurfaceStuffingWidget(QWidget *parent);
  void update();

  void generateTetrahedra(QGLTetraViewer *viewer_);

  QHBoxLayout cellSizeLayout;
  QLabel cellSizeLabel;
  QDoubleSpinBox cellSizeSpinBox;
  QHBoxLayout alphaLayout;
  QLabel alphaShortLabel;
  QDoubleSpinBox alphaShortSpinBox;
  QLabel alphaLongLabel;
  QDoubleSpinBox alphaLongSpinBox;
  QCheckBox gradedCheckBox;
  QPushButton generateTetrahedraButton;
};

#endif  // GENERIC_ISOSURFACE_STUFFING_WIDGET_HPP
