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
#include <QLoggingCategory>
#include <QStandardPaths>
#include <QtmGLViewer/QtmGLViewer.hpp>

int main(int argc, char *argv[]) {
  // QTetraMesher started by itself to run a tetrahedralization method
  if (TetraWorker::isWorkerInvocation(argc, argv))
    return TetraWorker::run(argc, argv);

  // OpenGL 3.3 core profile, must be set before creating the QApplication
  QtmGLViewer::setDefaultSurfaceFormat();
  QApplication a(argc, argv);
  // KDE's native file dialog warns about every file hidden by the name
  // filter ("No node found for item that was just removed"). QT_LOGGING_RULES
  // still overrides this.
  QLoggingCategory::setFilterRules("kf.kio.widgets.kdirmodel=false");
  // used by QSettings to find the config file
  QCoreApplication::setOrganizationName("dennis2society");
  QCoreApplication::setApplicationName("QTetraMesher");
  // The desktop file name is the app ID, e.g. for the window icon on
  // Wayland. Qt registers it with the xdg desktop portal, which fails with a
  // warning if no matching .desktop file is installed, so only set it then.
  if (!QStandardPaths::locate(QStandardPaths::ApplicationsLocation,
                              "qtetramesher.desktop")
           .isEmpty())
    QGuiApplication::setDesktopFileName("qtetramesher");
  QIcon icon;
  for (const int size : {16, 32, 48, 64, 128, 256})
    icon.addFile(QString(":/icons/qtetramesher_%1.png").arg(size));
  a.setWindowIcon(icon);
  QTetraMesherMainWindow w;
  w.show();

  return a.exec();
}
