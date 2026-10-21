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

  /// Called after a successful generation with the values the method chose
  /// automatically (keys as in parameters()). Fields that are still set to
  /// "auto" can show them, so they are a starting point for adjustments.
  virtual void applyUsedParameters(const QVariantMap &) {}

  /// switches the generate button between "Generate" and "Cancel"
  /// while a job runs, the generate button cancels it
  void setRunning(bool running,
                  const QString &cancelText = "Cancel Generation") {
    generateButton().setText(running ? cancelText : "Generate Tetrahedra");
  }
};

#endif  // TETRA_METHOD_WIDGET_HPP
