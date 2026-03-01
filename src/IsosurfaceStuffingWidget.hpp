/*
 * IsosurfaceStuffingWidget.hpp
 *
 * This widget provides options for the Isosurface Stuffing implementation
 * (Shewchuk's algorithm, self-contained C++ implementation).
 *
 *  Created 2026
 */
#ifndef ISOSURFACE_STUFFING_WIDGET_HPP
#define ISOSURFACE_STUFFING_WIDGET_HPP

#include <qgltetraviewer.hpp>

#include <BaseOptionsWidget.hpp>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QPushButton>

class IsosurfaceStuffingWidget : public BaseOptionsWidget {
  Q_OBJECT

 public:
  IsosurfaceStuffingWidget(QWidget *parent);
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

#endif  // ISOSURFACE_STUFFING_WIDGET_HPP
