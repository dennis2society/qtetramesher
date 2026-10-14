/*
 * main.cpp
 *
 *  Created 2011
 *      Author: Dennis Luebke
 */

#include "TetraJob.hpp"
#include "mainwindow.hpp"
#include <QApplication>
#include <QIcon>
#include <QtmGLViewer/QtmGLViewer.hpp>

int main(int argc, char *argv[]) {
  // QTetraMesher started by itself to run a tetrahedralization method
  if (TetraWorker::isWorkerInvocation(argc, argv))
    return TetraWorker::run(argc, argv);

  // OpenGL 3.3 core profile, must be set before creating the QApplication
  QtmGLViewer::setDefaultSurfaceFormat();
  QApplication a(argc, argv);
  // used by QSettings to find the config file
  QCoreApplication::setOrganizationName("dennis2society");
  QCoreApplication::setApplicationName("QTetraMesher");
  QGuiApplication::setDesktopFileName("qtetramesher");
  QIcon icon;
  for (const int size : {16, 32, 48, 64, 128, 256})
    icon.addFile(QString(":/icons/qtetramesher_%1.png").arg(size));
  a.setWindowIcon(icon);
  QTetraMesherMainWindow w;
  w.show();

  return a.exec();
}
