/*
 * octreeVisWidget.hpp
 *
 * This widget provides the UI options for the octree visualization.
 *
 *  Created 2024
 *      Author: Dennis Luebke
 */
#ifndef OCTREE_VIS_WIDGET_HPP
#define OCTREE_VIS_WIDGET_HPP

#include <BaseOptionsWidget.hpp>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QSpinBox>
#include <QLayout>

class OctreeVisWidget : public BaseOptionsWidget
{
  Q_OBJECT

public:
  OctreeVisWidget(QWidget *parent);
  void update();

  QComboBox octreeVisComboBox;
  QLabel octreeVisOptionsLabel;
  QHBoxLayout innerLayout;
  QLabel octreeDepthLabel;
  QSpinBox octreeDepthSpinBox;
  QPushButton octreeGenerateButton;
};

#endif // OCTREE_VIS_WIDGET_HPP
