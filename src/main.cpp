/*
 * main.cpp
 *
 *  Created 2011
 *      Author: Dennis Luebke
 */

#include "mainwindow.hpp"
#include <QApplication>
#include <QtmGLViewer/QtmGLViewer.hpp>

int main(int argc, char *argv[]) {
  // OpenGL 3.3 core profile, must be set before creating the QApplication
  QtmGLViewer::setDefaultSurfaceFormat();
  QApplication a(argc, argv);
  QTetraMesherMainWindow w;
  w.show();

  return a.exec();
}
