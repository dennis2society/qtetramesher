#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "cgaltetrahedralizedialog.h"
#include "tetrastuffingdialog.h"
#include <QCloseEvent>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void toggleFullScreen(bool value);
    void showTetraStuffingDialog();
    void showCGALDialog();
    void displayMessage(QString msg_, unsigned int time_);
    void notifyDone(QString msg_, unsigned int time_);
    void clearTetraOptions();   // call this when loading a new mesh to reset the settings

private:
    Ui::MainWindow *ui;
    TetraStuffingDialog* tsd;
    CGALTetrahedralizeDialog* ctd;

protected:
    // override main window close event to close all child dialogs
    void closeEvent(QCloseEvent * event);
};

#endif // MAINWINDOW_H
