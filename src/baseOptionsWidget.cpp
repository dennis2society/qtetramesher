/**
 * baseOptionsWidget.cpp
 *
 *  Created 2024
 *      Author: Dennis Luebke
 */

#include <baseOptionsWidget.hpp>

BaseOptionsWidget::BaseOptionsWidget(QWidget *parent) {
  this->setLayout(&layout);
  layout.stretch(-1);
}

