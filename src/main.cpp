/*
 * main.cpp
 *
 *  Created 2011
 *      Author: Dennis Luebke
 */

#include "mainwindow.hpp"
#include <QApplication>

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  QTetraMesherMainWindow w;
  w.show();

  return a.exec();
}
