/*
 * TetraMethodWidget.hpp
 *
 * Common interface of the option widgets of the tetrahedralization methods.
 * The widgets only collect the parameters; the generation itself runs in a
 * separate worker process (see TetraJob).
 */

#ifndef TETRA_METHOD_WIDGET_HPP
#define TETRA_METHOD_WIDGET_HPP

#include <BaseOptionsWidget.hpp>
#include <QPushButton>
#include <QString>
#include <QVariantMap>

class TetraMethodWidget : public BaseOptionsWidget {
 public:
  explicit TetraMethodWidget(QWidget *parent) : BaseOptionsWidget(parent) {}

  /// identifier understood by the worker process (see TetraJob.cpp)
  virtual QString methodId() const = 0;
  /// name shown to the user
  virtual QString methodName() const = 0;
  /// current parameter values, keys as expected by the worker process
  virtual QVariantMap parameters() const = 0;
  /// first line of the error dialog when the generation fails
  virtual QString errorHint() const = 0;

  virtual QPushButton &generateButton() = 0;

  /// switches the generate button between "Generate" and "Cancel"
  void setRunning(bool running) {
    generateButton().setText(running ? "Cancel Generation"
                                     : "Generate Tetrahedra");
  }
};

#endif  // TETRA_METHOD_WIDGET_HPP
