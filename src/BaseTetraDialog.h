/**
 * BaseTetraDialog.h
 *
 *  Base class for tetra meshing dialogs
 *  This will provide some basic functions necessary for the tetra meshing
 * dialog functionality, such as signals and slots.
 *
 *  Created 2012
 *      Author: Dennis Luebke
 */

#ifndef BASETETRADIALOG_H
#define BASETETRADIALOG_H

#include "qgltetraviewer.h"
#include <QDialog>
#include <QObject>

namespace Ui {
class BaseTetraDialog;
}

class BaseTetraDialog : public QDialog {
  Q_OBJECT

public:
  explicit BaseTetraDialog(QWidget *parent = 0);
  ~BaseTetraDialog();
  void setViewer(QGLTetraViewer *viewer_);

protected:
  QGLTetraViewer *_viewer;

protected slots:
  virtual void generateTetras() = 0;

signals:
  void notifyDone(const QString msg_, const unsigned int time_);
  void displayMessage(const QString msg_, const unsigned int time_);
};

#endif // BASETETRADIALOG_H
