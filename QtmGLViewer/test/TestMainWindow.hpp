/*
 * TestMainWindow.hpp
 *
 * Main window of the QtmGLViewer test application.
 */

#ifndef QTMGLVIEWER_TEST_TESTMAINWINDOW_HPP_
#define QTMGLVIEWER_TEST_TESTMAINWINDOW_HPP_

#include <QMainWindow>

class MobiusViewer;

class TestMainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit TestMainWindow(QWidget *parent = nullptr);

  MobiusViewer *viewer() const { return viewer_; }

 private:
  MobiusViewer *viewer_;
};

#endif /* QTMGLVIEWER_TEST_TESTMAINWINDOW_HPP_ */
