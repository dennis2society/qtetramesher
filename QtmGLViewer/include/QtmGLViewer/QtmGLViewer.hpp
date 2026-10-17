/*
 * QtmGLViewer.hpp
 *
 * Lightweight replacement for libQGLViewer based on QOpenGLWidget and the
 * OpenGL 3.3 core profile.
 *
 * Subclass it and override draw() (and initializeScene()/cleanupScene() for
 * GPU resources). The viewer provides a trackball camera, automatic scene
 * fitting, grid/axis drawing, FPS display and a text overlay.
 *
 * Default mouse bindings (same as libQGLViewer):
 *   left drag: rotate around the pivot    left double click: align axes
 *   right drag: translate                 right double click: center scene
 *   middle drag / wheel: zoom             middle double click: show scene
 * Keys: A = toggle axis, G = toggle grid, F = toggle FPS,
 *       O = toggle orthographic/perspective projection
 *
 * GPU resources: initializeScene() and cleanupScene() are called with the
 * context current, also when the context gets recreated (e.g. after
 * reparenting). Since virtual functions cannot be called from the base
 * class destructor, subclasses owning GPU resources must release them in
 * their own destructor:
 *   ~MyViewer() { makeCurrent(); cleanupScene(); doneCurrent(); }
 */

#ifndef QTMGLVIEWER_QTMGLVIEWER_HPP_
#define QTMGLVIEWER_QTMGLVIEWER_HPP_

#include <QColor>
#include <QElapsedTimer>
#include <QOpenGLWidget>
#include <QPointF>
#include <QQuaternion>
#include <QSurfaceFormat>
#include <QTimer>
#include <memory>
#include <vector>

#include "QtmGLViewer/Camera.hpp"
#include "QtmGLViewer/MeshBuffer.hpp"
#include "QtmGLViewer/RenderContext.hpp"

class QPainter;

class QtmGLViewer : public QOpenGLWidget {
  Q_OBJECT

 public:
  explicit QtmGLViewer(QWidget *parent = nullptr);
  ~QtmGLViewer() override;

  // OpenGL 3.3 core profile format with depth buffer and multisampling.
  static QSurfaceFormat defaultFormat();
  // Sets defaultFormat() as application wide default. Call this before
  // constructing the QApplication.
  static void setDefaultSurfaceFormat();

  qtm::Camera *camera() { return &camera_; }
  const qtm::Camera *camera() const { return &camera_; }

  QVector3D sceneCenter() const { return camera_.sceneCenter(); }
  float sceneRadius() const { return camera_.sceneRadius(); }

  bool axisIsDrawn() const { return axisIsDrawn_; }
  bool gridIsDrawn() const { return gridIsDrawn_; }
  bool FPSIsDisplayed() const { return fpsIsDisplayed_; }
  bool isOrthographic() const;
  bool spinningEnabled() const { return spinningEnabled_; }
  float currentFPS() const { return fps_; }

  QColor backgroundColor() const { return backgroundColor_; }
  QColor foregroundColor() const { return foregroundColor_; }

  // Queues a text for the overlay of the current frame. Call it from
  // draw(). (x, y) is the baseline start in widget coordinates. An invalid
  // color means foregroundColor().
  void drawText(int x, int y, const QString &text,
                const QColor &color = QColor());

  // true if OpenGL 3.3 core and the built-in shaders are available
  bool isGLReady() const { return glReady_; }

 public slots:
  void setSceneCenter(const QVector3D &center);
  void setSceneRadius(float radius);
  void showEntireScene();
  void centerScene();
  void alignCamera();

  void setAxisIsDrawn(bool drawn = true);
  void toggleAxisIsDrawn() { setAxisIsDrawn(!axisIsDrawn_); }
  void setGridIsDrawn(bool drawn = true);
  void toggleGridIsDrawn() { setGridIsDrawn(!gridIsDrawn_); }
  void setFPSIsDisplayed(bool displayed = true);
  void toggleFPSIsDisplayed() { setFPSIsDisplayed(!fpsIsDisplayed_); }
  void setOrthographic(bool orthographic);
  void toggleCameraType() { setOrthographic(!isOrthographic()); }

  // Keeps rotating after a fast mouse release (off by default).
  void setSpinningEnabled(bool enabled);
  void stopSpinning();

  void setBackgroundColor(const QColor &color);
  void setForegroundColor(const QColor &color);

  // Shows a message at the bottom left for delayMs milliseconds.
  void displayMessage(const QString &message, int delayMs = 2000);

 signals:
  void axisIsDrawnChanged(bool drawn);
  void gridIsDrawnChanged(bool drawn);
  void FPSIsDisplayedChanged(bool displayed);
  void orthographicChanged(bool orthographic);

 protected:
  // Hooks for subclasses. The OpenGL context is current in all of them.
  virtual void initializeScene() {}
  virtual void cleanupScene() {}
  // Draws the scene in world coordinates. Depth test is enabled, face
  // culling and blending are disabled when it is called.
  virtual void draw(const qtm::RenderContext &context);
  // Additional 2D drawing on top of the 3D scene.
  virtual void drawOverlay(QPainter &painter);

  const qtm::RenderContext &renderContext() const { return *context_; }

  void initializeGL() override;
  void resizeGL(int width, int height) override;
  void paintGL() override;

  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void mouseDoubleClickEvent(QMouseEvent *event) override;
  void wheelEvent(QWheelEvent *event) override;
  void keyPressEvent(QKeyEvent *event) override;

 private slots:
  void cleanupGL();
  void spinStep();

 private:
  enum class MouseAction { None, Rotate, Translate, Zoom };

  void releaseGLResources();
  void drawVisualHints();
  void drawTextOverlay();
  void updateFPS();

  qtm::Camera camera_;
  std::unique_ptr<qtm::RenderContext> context_;
  qtm::MeshBuffer gridMesh_;
  qtm::MeshBuffer axisLettersMesh_;
  qtm::MeshBuffer axisArrowsMesh_;
  bool glReady_ = false;

  bool axisIsDrawn_ = false;
  bool gridIsDrawn_ = false;
  bool fpsIsDisplayed_ = false;
  QColor backgroundColor_{51, 51, 51};
  QColor foregroundColor_{180, 180, 180};

  struct OverlayText {
    int x;
    int y;
    QString text;
    QColor color;
  };
  std::vector<OverlayText> overlayTexts_;
  QString message_;
  QTimer messageTimer_;

  QElapsedTimer fpsTimer_;
  int fpsCounter_ = 0;
  float fps_ = 0.0f;

  // mouse interaction
  MouseAction mouseAction_ = MouseAction::None;
  QPointF prevPos_;
  float rotationSensitivity_ = 1.0f;
  float zoomSensitivity_ = 1.0f;
  float wheelSensitivity_ = 1.0f;

  // spinning
  bool spinningEnabled_ = false;
  float spinningSensitivity_ = 0.3f;  // pixels per millisecond
  float mouseSpeed_ = 0.0f;
  QElapsedTimer lastMoveTimer_;
  QQuaternion spinningRotation_;
  QTimer spinTimer_;
};

#endif /* QTMGLVIEWER_QTMGLVIEWER_HPP_ */
