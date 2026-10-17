/*
 * main.cpp
 *
 * QtmGLViewer test application.
 */

#include <QApplication>
#include <QCommandLineParser>
#include <QTimer>
#include <QtmGLViewer/QtmGLViewer.hpp>

#include "SphereViewer.hpp"
#include "TestMainWindow.hpp"

int main(int argc, char *argv[]) {
  // must happen before the QApplication is created
  QtmGLViewer::setDefaultSurfaceFormat();
  QApplication app(argc, argv);

  QCommandLineParser parser;
  parser.addHelpOption();
  QCommandLineOption screenshotOption(
      QStringLiteral("screenshot"),
      QStringLiteral("Saves a screenshot of the viewer to <file> and exits."),
      QStringLiteral("file"));
  parser.addOption(screenshotOption);
  parser.process(app);

  TestMainWindow window;
  window.show();

  if (parser.isSet(screenshotOption)) {
    const QString file = parser.value(screenshotOption);
    QTimer::singleShot(1000, &app, [&window, file]() {
      const bool ok = window.viewer()->grabFramebuffer().save(file);
      QCoreApplication::exit(ok ? 0 : 1);
    });
  }
  return app.exec();
}
