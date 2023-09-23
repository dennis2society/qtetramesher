/*
 * cgaltetrahedralizedialog.h
 *
 *  Created 2012
 *      Author: Dennis Luebke
 */

#ifndef CGALTETRAHEDRALIZEDIALOG_H
#define CGALTETRAHEDRALIZEDIALOG_H

#include <BaseTetraDialog.h>

namespace Ui {
class CGALTetrahedralizeDialog;
}

class CGALTetrahedralizeDialog : public BaseTetraDialog {
  Q_OBJECT

public:
  explicit CGALTetrahedralizeDialog(QWidget *parent = 0);
  ~CGALTetrahedralizeDialog();
  void setParams(const float tetraSize_,
                 const float faceSize_); // set recommended tetra size depending
                                         // on surface mesh size

private:
  Ui::CGALTetrahedralizeDialog *m_ui;

private slots:
  virtual void generateTetras();
};

#endif // CGALTETRAHEDRALIZEDIALOG_H
