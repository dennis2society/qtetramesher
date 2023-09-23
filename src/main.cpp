/*
 * main.cpp
 *
 *  Created 2011
 *      Author: Dennis Luebke
 */

#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  MainWindow w;
  w.show();

  return a.exec();
}
