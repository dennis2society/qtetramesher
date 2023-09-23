/*
 * tetrastuffingdialog.h
 *
 *  Created 2012
 *      Author: Dennis Luebke
 */

#ifndef TETRASTUFFINGDIALOG_H
#define TETRASTUFFINGDIALOG_H

#include <BaseTetraDialog.h>

namespace Ui {
class TetraStuffingDialog;
}

class TetraStuffingDialog : public BaseTetraDialog {
  Q_OBJECT

public:
  explicit TetraStuffingDialog(QWidget *parent = 0);
  ~TetraStuffingDialog();

public slots:
  void setTetraSize(const float tetraSize_); // set recommended tetra size
                                             // depending on surface mesh size

protected:
  Ui::TetraStuffingDialog *m_ui;

private slots:
  virtual void generateTetras();
};

#endif // TETRASTUFFINGDIALOG_H
