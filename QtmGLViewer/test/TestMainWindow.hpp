/*
 * TestMainWindow.hpp
 *
 * Main window of the QtmGLViewer test application.
 */

#ifndef QTMGLVIEWER_TEST_TESTMAINWINDOW_HPP_
#define QTMGLVIEWER_TEST_TESTMAINWINDOW_HPP_

#include <QMainWindow>

class SphereViewer;

class TestMainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit TestMainWindow(QWidget *parent = nullptr);

  SphereViewer *viewer() const { return viewer_; }

 private:
  SphereViewer *viewer_;
};

#endif /* QTMGLVIEWER_TEST_TESTMAINWINDOW_HPP_ */
