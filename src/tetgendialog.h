/*
 * cgaltetrahedralizedialog.h
 *
 *  Created 2012
 *      Author: Dennis Luebke
 */

#pragma once

#include <BaseTetraDialog.h>

namespace Ui {
class TetgenDialog;
}

class TetgenDialog : public BaseTetraDialog {
  Q_OBJECT

public:
  explicit TetgenDialog(QWidget *parent = 0);
  ~TetgenDialog();
  void setParams(const float volumeConstraint,
                 const float qualityBound,
                 bool usePLC);

private:
  Ui::TetgenDialog *m_ui;

private slots:
  virtual void generateTetras();
};
