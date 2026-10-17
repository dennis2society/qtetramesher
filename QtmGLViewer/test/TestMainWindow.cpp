/*
 * TestMainWindow.cpp
 */

#include "TestMainWindow.hpp"

#include <QCheckBox>
#include <QColorDialog>
#include <QDockWidget>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

#include "SphereViewer.hpp"

TestMainWindow::TestMainWindow(QWidget *parent) : QMainWindow(parent) {
  setWindowTitle(tr("QtmGLViewer Test"));
  resize(1100, 750);

  viewer_ = new SphereViewer(this);
  viewer_->setAxisIsDrawn(true);
  viewer_->setGridIsDrawn(true);
  viewer_->setFPSIsDisplayed(true);
  setCentralWidget(viewer_);

  auto *panel = new QWidget;
  auto *layout = new QVBoxLayout(panel);

  auto *axisBox = new QCheckBox(tr("Show axis (A)"));
  axisBox->setChecked(viewer_->axisIsDrawn());
  connect(axisBox, &QCheckBox::toggled, viewer_, &QtmGLViewer::setAxisIsDrawn);
  connect(viewer_, &QtmGLViewer::axisIsDrawnChanged, axisBox,
          &QCheckBox::setChecked);

  auto *gridBox = new QCheckBox(tr("Show grid (G)"));
  gridBox->setChecked(viewer_->gridIsDrawn());
  connect(gridBox, &QCheckBox::toggled, viewer_, &QtmGLViewer::setGridIsDrawn);
  connect(viewer_, &QtmGLViewer::gridIsDrawnChanged, gridBox,
          &QCheckBox::setChecked);

  auto *fpsBox = new QCheckBox(tr("Show FPS (F)"));
  fpsBox->setChecked(viewer_->FPSIsDisplayed());
  connect(fpsBox, &QCheckBox::toggled, viewer_,
          &QtmGLViewer::setFPSIsDisplayed);
  connect(viewer_, &QtmGLViewer::FPSIsDisplayedChanged, fpsBox,
          &QCheckBox::setChecked);

  auto *orthoBox = new QCheckBox(tr("Orthographic (O)"));
  connect(orthoBox, &QCheckBox::toggled, viewer_,
          &QtmGLViewer::setOrthographic);
  connect(viewer_, &QtmGLViewer::orthographicChanged, orthoBox,
          &QCheckBox::setChecked);

  auto *spinBox = new QCheckBox(tr("Spinning after release"));
  connect(spinBox, &QCheckBox::toggled, viewer_,
          &QtmGLViewer::setSpinningEnabled);

  auto *wireBox = new QCheckBox(tr("Sphere wireframe"));
  connect(wireBox, &QCheckBox::toggled, viewer_,
          &SphereViewer::setWireframeDrawn);

  for (QCheckBox *box :
       {axisBox, gridBox, fpsBox, orthoBox, spinBox, wireBox})
    layout->addWidget(box);

  auto *form = new QFormLayout;
  auto *radiusEdit = new QDoubleSpinBox;
  radiusEdit->setDecimals(3);
  radiusEdit->setRange(0.001, 10000.0);
  radiusEdit->setValue(viewer_->sphereRadius());
  radiusEdit->setStepType(QAbstractSpinBox::AdaptiveDecimalStepType);
  connect(radiusEdit, &QDoubleSpinBox::valueChanged, viewer_,
          &SphereViewer::setSphereRadius);
  form->addRow(tr("Radius:"), radiusEdit);

  auto *subdivEdit = new QSpinBox;
  subdivEdit->setRange(0, 7);
  subdivEdit->setValue(3);
  connect(subdivEdit, &QSpinBox::valueChanged, viewer_,
          &SphereViewer::setSubdivisions);
  form->addRow(tr("Subdivisions:"), subdivEdit);
  layout->addLayout(form);

  auto *colorButton = new QPushButton(tr("Sphere color..."));
  connect(colorButton, &QPushButton::clicked, this, [this]() {
    const QColor c = QColorDialog::getColor(Qt::lightGray, this);
    if (c.isValid()) viewer_->setSphereColor(c);
  });
  layout->addWidget(colorButton);

  auto *fitButton = new QPushButton(tr("Show entire scene"));
  connect(fitButton, &QPushButton::clicked, viewer_,
          &QtmGLViewer::showEntireScene);
  layout->addWidget(fitButton);

  auto *messageButton = new QPushButton(tr("Display message"));
  connect(messageButton, &QPushButton::clicked, viewer_, [this]() {
    viewer_->displayMessage(tr("This message disappears in 3 seconds"), 3000);
  });
  layout->addWidget(messageButton);

  auto *help = new QLabel(
      tr("<b>Mouse</b><br>"
         "Left drag: rotate<br>"
         "Right drag: translate<br>"
         "Middle drag / wheel: zoom<br><br>"
         "<b>Double click</b><br>"
         "Left: align with axes<br>"
         "Right: center scene<br>"
         "Middle: show entire scene"));
  help->setWordWrap(true);
  layout->addWidget(help);
  layout->addStretch();

  auto *dock = new QDockWidget(tr("Options"), this);
  dock->setFeatures(QDockWidget::NoDockWidgetFeatures);
  dock->setWidget(panel);
  addDockWidget(Qt::LeftDockWidgetArea, dock);
}
