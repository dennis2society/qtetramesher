/*
 * cgaltetrahedralizedialog.h
 *
 *  Created 2012
 *      Author: Dennis Luebke
 */

#ifndef QUARTETTETRAHEDRALIZEDIALOG_H
#define QUARTETTETRAHEDRALIZEDIALOG_H

#include <BaseTetraDialog.h>

namespace Ui {
class QuartetTetraStuffingDialog;
}

class QuartetTetraStuffingDialog : public BaseTetraDialog {
  Q_OBJECT

public:
  explicit QuartetTetraStuffingDialog(QWidget *parent = 0);
  ~QuartetTetraStuffingDialog();
  void setParams(const float gridSpacing,
                 const float angleThreshold); // set recommended tetra size
                                              // depending on surface mesh size

private:
  Ui::QuartetTetraStuffingDialog *m_ui;

private slots:
  virtual void generateTetras();
};

#endif // QUARTETTETRAHEDRALIZEDIALOG_H
