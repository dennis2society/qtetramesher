/*
 * mainwindow.cpp
 *
 *  Created 2012
 *      Author: Dennis Luebke
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->viewer->setFPSIsDisplayed(true);
    ui->viewer->setAxisIsDrawn(true);
    ui->viewer->setGridIsDrawn(true);
    tsd = NULL;
    ctd = NULL;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::toggleFullScreen(bool value)
{
    if (value)
    {
        this->showFullScreen();
    }
    else
    {
        this->showNormal();
    }
}

void MainWindow::showTetraStuffingDialog()
{
    if (tsd == NULL)
    {
        tsd = new TetraStuffingDialog();
        tsd->setViewer(ui->viewer);
        connect(tsd, SIGNAL(displayMessage(QString,uint)), this, SLOT(displayMessage(QString,uint)));
        connect(tsd, SIGNAL(notifyDone(QString,uint)), this, SLOT(notifyDone(QString,uint)));
    }
    tsd->setTetraSize(ui->viewer->getMaxBBox() * 0.05f);
    tsd->show();
}

void MainWindow::showCGALDialog()
{
    if (ctd == NULL)
    {
        //std::cout<<"Creating CGAL Dialog..."<<std::endl;
        ctd = new CGALTetrahedralizeDialog();
        ctd->setViewer(ui->viewer);
        connect(ctd, SIGNAL(displayMessage(QString,uint)), this, SLOT(displayMessage(QString,uint)));
        connect(ctd, SIGNAL(notifyDone(QString,uint)), this, SLOT(notifyDone(QString,uint)));
    }
    ctd->setParams(ui->viewer->getMaxBBox() * 0.1f, ui->viewer->getMaxBBox() * 0.05);
    ctd->show();
}

void MainWindow::generateOctree()
{
    ui->viewer->generateOctree(ui->octreeDepthSpinbox->value());
    ui->viewer->updateGL();
}

void MainWindow::displayMessage(QString msg_, unsigned int time_)
{
    ui->viewer->ShowStatusMessage(msg_, time_);
}

void MainWindow::notifyDone(QString msg_, unsigned int time_)
{
    ui->viewer->ShowStatusMessage(msg_, time_);
    ui->viewer->updateGL();
}

void MainWindow::clearTetraOptions()
{
    if (tsd != NULL)
    {
        tsd->hide();
        delete tsd;
        tsd = NULL;
    }
    if (ctd != NULL)
    {
        ctd->hide();
        delete ctd;
        ctd = NULL;
    }
}

void MainWindow::closeEvent(QCloseEvent * event)
{
    clearTetraOptions();
}

void MainWindow::on_viewer_onLoad()
{

}
