/*
 * SurfaceTransformWidget.cpp
 *
 * Sliders for scaling and rotating the loaded surface mesh.
 */

#include <QFont>
#include <QSignalBlocker>
#include <SurfaceTransformWidget.hpp>
#include <cmath>

namespace {
// the scale slider is logarithmic: value = 100 * log10(scale)
const int kScaleSliderRange = 400;  // 1e-4 .. 1e4
const double kMinScale = 1e-4;
const double kMaxScale = 1e4;

int scaleToSlider(double s) { return int(std::lround(std::log10(s) * 100.0)); }
}  // namespace

SurfaceTransformWidget::SurfaceTransformWidget(QWidget *parent)
    : BaseOptionsWidget(parent) {
  labelTitle.setText("Surface Transform");
  QFont boldFont;
  boldFont.setBold(true);
  labelTitle.setFont(boldFont);
  setToolTip(
      "Scale and rotate the loaded surface mesh around its center. The "
      "original mesh is kept, 'Reset Transform' restores it. A generated "
      "tetra mesh is removed when the surface changes.");

  scaleLabel.setText("Scale");
  scaleSlider.setOrientation(Qt::Horizontal);
  scaleSlider.setRange(-kScaleSliderRange, kScaleSliderRange);
  scaleSlider.setPageStep(100);  // factor 10
  scaleSlider.setValue(0);
  scaleSpinBox.setRange(kMinScale, kMaxScale);
  scaleSpinBox.setDecimals(4);
  scaleSpinBox.setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
  scaleSpinBox.setKeyboardTracking(false);
  scaleSpinBox.setValue(1.0);
  grid.addWidget(&scaleLabel, 0, 0);
  grid.addWidget(&scaleSlider, 0, 1);
  grid.addWidget(&scaleSpinBox, 0, 2);

  connect(&scaleSlider, &QSlider::valueChanged, this, [this](int v) {
    {
      const QSignalBlocker blocker(scaleSpinBox);
      scaleSpinBox.setValue(std::pow(10.0, v / 100.0));
    }
    emit transformChanged(true, !scaleSlider.isSliderDown());
  });
  connect(&scaleSlider, &QSlider::sliderReleased, this,
          [this]() { emit transformChanged(true, true); });
  connect(&scaleSpinBox, &QDoubleSpinBox::valueChanged, this,
          [this](double s) {
            {
              const QSignalBlocker blocker(scaleSlider);
              scaleSlider.setValue(scaleToSlider(s));
            }
            emit transformChanged(true, true);
          });

  const char *axes[3] = {"Rotate X", "Rotate Y", "Rotate Z"};
  for (int i = 0; i < 3; ++i) {
    QSlider &slider = rotationSliders[i];
    QSpinBox &spinBox = rotationSpinBoxes[i];
    rotationLabels[i].setText(axes[i]);
    slider.setOrientation(Qt::Horizontal);
    slider.setRange(-180, 180);
    slider.setPageStep(45);
    spinBox.setRange(-180, 180);
    spinBox.setSuffix(QString::fromUtf8("\u00b0"));
    spinBox.setWrapping(true);
    spinBox.setKeyboardTracking(false);
    grid.addWidget(&rotationLabels[i], i + 1, 0);
    grid.addWidget(&slider, i + 1, 1);
    grid.addWidget(&spinBox, i + 1, 2);
    connect(&slider, &QSlider::valueChanged, this, [this, i](int v) {
      {
        const QSignalBlocker blocker(rotationSpinBoxes[i]);
        rotationSpinBoxes[i].setValue(v);
      }
      emit transformChanged(false, !rotationSliders[i].isSliderDown());
    });
    connect(&slider, &QSlider::sliderReleased, this,
            [this]() { emit transformChanged(false, true); });
    connect(&spinBox, &QSpinBox::valueChanged, this, [this, i](int v) {
      {
        const QSignalBlocker blocker(rotationSliders[i]);
        rotationSliders[i].setValue(v);
      }
      emit transformChanged(false, true);
    });
  }

  resetButton.setText("Reset Transform");
  resetButton.setMaximumHeight(20);
  layout.addWidget(&labelTitle);
  layout.addLayout(&grid);
  layout.addWidget(&resetButton);
}

void SurfaceTransformWidget::resetValues() {
  const QSignalBlocker b0(scaleSlider), b1(scaleSpinBox);
  scaleSlider.setValue(0);
  scaleSpinBox.setValue(1.0);
  for (int i = 0; i < 3; ++i) {
    const QSignalBlocker b2(rotationSliders[i]), b3(rotationSpinBoxes[i]);
    rotationSliders[i].setValue(0);
    rotationSpinBoxes[i].setValue(0);
  }
}
