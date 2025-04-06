/**
 * baseOptionsWidget.cpp
 *
 *  Created 2024
 *      Author: Dennis Luebke
 */

#include <BaseOptionsWidget.hpp>

BaseOptionsWidget::BaseOptionsWidget(QWidget *parent) {
  this->setLayout(&layout);
  layout.stretch(-1);
}

