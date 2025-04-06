/*
 * baseOptionsWidget.h
 *
 *  Created 2024
 *      Author: Dennis Luebke
 */

#ifndef BASE_OPTIONS_WIDGET_H
#define BASE_OPTIONS_WIDGET_H

#include <QWidget>
#include <QLayout>
#include <QLabel>

class BaseOptionsWidget : public QWidget
{
  Q_OBJECT

public:
  BaseOptionsWidget(QWidget *parent);

  void setupUI();
  void connectSlots();

protected:
  QVBoxLayout layout;
  QLabel labelTitle;
};

#endif //BASE_OPTIONS_WIDGET_H
