/*
 * SurfaceTransformWidget.hpp
 *
 * Sliders for scaling and rotating the loaded surface mesh.
 */

#ifndef SURFACE_TRANSFORM_WIDGET_H
#define SURFACE_TRANSFORM_WIDGET_H

#include <BaseOptionsWidget.hpp>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>

class SurfaceTransformWidget : public BaseOptionsWidget {
  Q_OBJECT

 public:
  SurfaceTransformWidget(QWidget *parent);

  float scale() const { return float(scaleSpinBox.value()); }
  float rotationX() const { return float(rotationSpinBoxes[0].value()); }
  float rotationY() const { return float(rotationSpinBoxes[1].value()); }
  float rotationZ() const { return float(rotationSpinBoxes[2].value()); }
  /// Back to scale 1 and no rotation without emitting transformChanged().
  void resetValues();

  QPushButton resetButton;

 signals:
  /// scaleChanged_ is false for rotation-only changes. settled_ is false
  /// while a slider is being dragged.
  void transformChanged(bool scaleChanged_, bool settled_);

 private:
  QGridLayout grid;
  QLabel scaleLabel;
  QSlider scaleSlider;
  QDoubleSpinBox scaleSpinBox;
  QLabel rotationLabels[3];
  QSlider rotationSliders[3];
  QSpinBox rotationSpinBoxes[3];
};

#endif  // SURFACE_TRANSFORM_WIDGET_H
