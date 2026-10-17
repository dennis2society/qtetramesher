/*
 * QtmGLViewer.cpp
 */

#include "QtmGLViewer/QtmGLViewer.hpp"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QOpenGLContext>
#include <QOpenGLExtraFunctions>
#include <QPainter>
#include <QWheelEvent>
#include <cmath>

#include "QtmGLViewer/MeshData.hpp"

QtmGLViewer::QtmGLViewer(QWidget *parent) : QOpenGLWidget(parent) {
  setFormat(defaultFormat());
  setFocusPolicy(Qt::StrongFocus);
  setMinimumSize(100, 100);

  messageTimer_.setSingleShot(true);
  connect(&messageTimer_, &QTimer::timeout, this, [this]() {
    message_.clear();
    update();
  });
  spinTimer_.setInterval(16);
  connect(&spinTimer_, &QTimer::timeout, this, &QtmGLViewer::spinStep);
  fpsTimer_.start();
}

QtmGLViewer::~QtmGLViewer() {
  if (context())
    disconnect(context(), &QOpenGLContext::aboutToBeDestroyed, this,
               &QtmGLViewer::cleanupGL);
  makeCurrent();
  releaseGLResources();
  doneCurrent();
}

QSurfaceFormat QtmGLViewer::defaultFormat() {
  QSurfaceFormat format = QSurfaceFormat::defaultFormat();
  format.setRenderableType(QSurfaceFormat::OpenGL);
  format.setVersion(3, 3);
  format.setProfile(QSurfaceFormat::CoreProfile);
  format.setDepthBufferSize(24);
  if (format.samples() < 0) format.setSamples(4);
  return format;
}

void QtmGLViewer::setDefaultSurfaceFormat() {
  QSurfaceFormat::setDefaultFormat(defaultFormat());
}

bool QtmGLViewer::isOrthographic() const {
  return camera_.type() == qtm::Camera::Type::Orthographic;
}

void QtmGLViewer::drawText(int x, int y, const QString &text,
                           const QColor &color) {
  overlayTexts_.push_back(
      {x, y, text, color.isValid() ? color : foregroundColor_});
}

void QtmGLViewer::setSceneCenter(const QVector3D &center) {
  camera_.setSceneCenter(center);
  update();
}

void QtmGLViewer::setSceneRadius(float radius) {
  camera_.setSceneRadius(radius);
  update();
}

void QtmGLViewer::showEntireScene() {
  camera_.showEntireScene();
  update();
}

void QtmGLViewer::centerScene() {
  camera_.centerScene();
  update();
}

void QtmGLViewer::alignCamera() {
  stopSpinning();
  camera_.alignWithNearestAxis();
  update();
}

void QtmGLViewer::setAxisIsDrawn(bool drawn) {
  if (drawn == axisIsDrawn_) return;
  axisIsDrawn_ = drawn;
  emit axisIsDrawnChanged(drawn);
  update();
}

void QtmGLViewer::setGridIsDrawn(bool drawn) {
  if (drawn == gridIsDrawn_) return;
  gridIsDrawn_ = drawn;
  emit gridIsDrawnChanged(drawn);
  update();
}

void QtmGLViewer::setFPSIsDisplayed(bool displayed) {
  if (displayed == fpsIsDisplayed_) return;
  fpsIsDisplayed_ = displayed;
  emit FPSIsDisplayedChanged(displayed);
  update();
}

void QtmGLViewer::setOrthographic(bool orthographic) {
  if (orthographic == isOrthographic()) return;
  camera_.setType(orthographic ? qtm::Camera::Type::Orthographic
                               : qtm::Camera::Type::Perspective);
  emit orthographicChanged(orthographic);
  update();
}

void QtmGLViewer::setSpinningEnabled(bool enabled) {
  spinningEnabled_ = enabled;
  if (!enabled) stopSpinning();
}

void QtmGLViewer::stopSpinning() { spinTimer_.stop(); }

void QtmGLViewer::setBackgroundColor(const QColor &color) {
  backgroundColor_ = color;
  update();
}

void QtmGLViewer::setForegroundColor(const QColor &color) {
  foregroundColor_ = color;
  update();
}

void QtmGLViewer::displayMessage(const QString &message, int delayMs) {
  message_ = message;
  messageTimer_.start(delayMs);
  update();
}

void QtmGLViewer::draw(const qtm::RenderContext &) {}

void QtmGLViewer::drawOverlay(QPainter &) {}

void QtmGLViewer::initializeGL() {
  connect(context(), &QOpenGLContext::aboutToBeDestroyed, this,
          &QtmGLViewer::cleanupGL, Qt::UniqueConnection);

  const QSurfaceFormat format = context()->format();
  const bool versionOk =
      !context()->isOpenGLES() &&
      (format.majorVersion() > 3 ||
       (format.majorVersion() == 3 && format.minorVersion() >= 3));
  if (!versionOk) {
    qWarning("QtmGLViewer: OpenGL 3.3 is required, got %d.%d%s",
             format.majorVersion(), format.minorVersion(),
             context()->isOpenGLES() ? " ES" : "");
  }

  context_ = std::make_unique<qtm::RenderContext>();
  glReady_ = versionOk && context_->initialize();
  if (!glReady_) return;

  gridMesh_.upload(qtm::primitives::grid(1.0f, 10), GL_LINES);
  axisLettersMesh_.upload(qtm::primitives::axisLetters(1.0f), GL_LINES);

  // colors as in QGLViewer::drawAxis()
  const qtm::MeshData arrow = qtm::primitives::arrow(1.0f, 0.01f);
  qtm::MeshData arrows;
  qtm::MeshData part = arrow;
  part.setColor(QVector3D(0.7f, 0.7f, 1.0f));  // Z
  arrows.append(part);
  QMatrix4x4 toX;
  toX.rotate(90.0f, 0.0f, 1.0f, 0.0f);
  part.setColor(QVector3D(1.0f, 0.7f, 0.7f));  // X
  arrows.append(part, toX);
  QMatrix4x4 toY;
  toY.rotate(-90.0f, 1.0f, 0.0f, 0.0f);
  part.setColor(QVector3D(0.7f, 1.0f, 0.7f));  // Y
  arrows.append(part, toY);
  axisArrowsMesh_.upload(arrows, GL_TRIANGLES);

  initializeScene();
}

void QtmGLViewer::cleanupGL() {
  makeCurrent();
  if (glReady_) cleanupScene();
  releaseGLResources();
  doneCurrent();
}

void QtmGLViewer::releaseGLResources() {
  gridMesh_.destroy();
  axisLettersMesh_.destroy();
  axisArrowsMesh_.destroy();
  context_.reset();
  glReady_ = false;
}

void QtmGLViewer::resizeGL(int width, int height) {
  camera_.setScreenSize(width, height);
}

void QtmGLViewer::paintGL() {
  QOpenGLExtraFunctions *gl = context()->extraFunctions();
  // QPainter may have changed the state in the previous frame
  gl->glDisable(GL_SCISSOR_TEST);
  gl->glDisable(GL_STENCIL_TEST);
  gl->glDisable(GL_BLEND);
  gl->glDisable(GL_CULL_FACE);
  gl->glDepthMask(GL_TRUE);
  gl->glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  gl->glClearColor(float(backgroundColor_.redF()),
                   float(backgroundColor_.greenF()),
                   float(backgroundColor_.blueF()), 1.0f);
  gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (glReady_) {
    camera_.setScreenSize(width(), height());
    context_->beginFrame(camera_);
    gl->glEnable(GL_DEPTH_TEST);
    gl->glDepthFunc(GL_LEQUAL);

    draw(*context_);

    // reset what draw() might have changed
    gl->glEnable(GL_DEPTH_TEST);
    gl->glDepthMask(GL_TRUE);
    gl->glDisable(GL_CULL_FACE);
    gl->glDisable(GL_BLEND);
    gl->glDisable(GL_POLYGON_OFFSET_FILL);
    drawVisualHints();
  } else {
    drawText(10, 30,
             tr("OpenGL 3.3 core profile is not available - see console "
                "output."),
             QColor(255, 80, 80));
  }

  updateFPS();
  drawTextOverlay();
}

void QtmGLViewer::drawVisualHints() {
  QMatrix4x4 scale;
  scale.scale(camera_.sceneRadius());
  if (gridIsDrawn_) context_->drawLines(gridMesh_, foregroundColor_, scale);
  if (axisIsDrawn_) {
    context_->drawLines(axisLettersMesh_, foregroundColor_, scale);
    context_->drawSolid(axisArrowsMesh_, QColor(), scale);
  }
}

void QtmGLViewer::updateFPS() {
  // same as libQGLViewer: average over 20 frames
  const int maxCounter = 20;
  if (++fpsCounter_ == maxCounter) {
    const qint64 elapsed = fpsTimer_.restart();
    fps_ = elapsed > 0 ? 1000.0f * maxCounter / float(elapsed) : 0.0f;
    fpsCounter_ = 0;
  }
}

void QtmGLViewer::drawTextOverlay() {
  QPainter painter(this);
  painter.setRenderHint(QPainter::TextAntialiasing);
  painter.setFont(font());
  drawOverlay(painter);
  if (fpsIsDisplayed_) {
    // same position as libQGLViewer's displayFPS()
    const QFont &f = painter.font();
    const double fontSize = f.pixelSize() > 0 ? f.pixelSize() : f.pointSizeF();
    painter.setPen(foregroundColor_);
    painter.drawText(10, int(1.5 * fontSize),
                     QStringLiteral("%1Hz").arg(double(fps_), 0, 'f',
                                                fps_ < 10.0f ? 1 : 0));
  }
  for (const OverlayText &t : overlayTexts_) {
    painter.setPen(t.color);
    painter.drawText(t.x, t.y, t.text);
  }
  if (!message_.isEmpty()) {
    painter.setPen(foregroundColor_);
    painter.drawText(10, height() - 10, message_);
  }
  painter.end();
  overlayTexts_.clear();
}

void QtmGLViewer::mousePressEvent(QMouseEvent *event) {
  stopSpinning();
  prevPos_ = event->position();
  mouseSpeed_ = 0.0f;
  lastMoveTimer_.start();
  switch (event->button()) {
    case Qt::LeftButton:
      mouseAction_ = MouseAction::Rotate;
      break;
    case Qt::RightButton:
      mouseAction_ = MouseAction::Translate;
      break;
    case Qt::MiddleButton:
      mouseAction_ = MouseAction::Zoom;
      break;
    default:
      mouseAction_ = MouseAction::None;
      break;
  }
  event->accept();
}

void QtmGLViewer::mouseMoveEvent(QMouseEvent *event) {
  if (mouseAction_ == MouseAction::None) {
    QOpenGLWidget::mouseMoveEvent(event);
    return;
  }
  const QPointF pos = event->position();
  switch (mouseAction_) {
    case MouseAction::Rotate: {
      const QQuaternion rotation =
          camera_.trackballRotation(prevPos_, pos, rotationSensitivity_);
      camera_.rotateAroundPivot(rotation);
      spinningRotation_ = rotation;
      // mouse speed in pixels per millisecond, used for spinning
      const QPointF delta = pos - prevPos_;
      const float distance =
          float(std::sqrt(delta.x() * delta.x() + delta.y() * delta.y()));
      const qint64 delay = lastMoveTimer_.restart();
      mouseSpeed_ = delay == 0 ? distance : distance / float(delay);
      break;
    }
    case MouseAction::Translate:
      camera_.translate(camera_.panTranslation(prevPos_ - pos));
      break;
    case MouseAction::Zoom: {
      const float dx = float(pos.x() - prevPos_.x()) / float(width());
      const float dy = float(pos.y() - prevPos_.y()) / float(height());
      const float value = std::fabs(dx) > std::fabs(dy) ? dx : dy;
      camera_.zoom(value * zoomSensitivity_);
      break;
    }
    case MouseAction::None:
      break;
  }
  prevPos_ = pos;
  update();
  event->accept();
}

void QtmGLViewer::mouseReleaseEvent(QMouseEvent *event) {
  // only spin if the mouse was still moving when released
  if (mouseAction_ == MouseAction::Rotate && spinningEnabled_ &&
      mouseSpeed_ >= spinningSensitivity_ && lastMoveTimer_.elapsed() < 50)
    spinTimer_.start();
  mouseAction_ = MouseAction::None;
  event->accept();
}

void QtmGLViewer::mouseDoubleClickEvent(QMouseEvent *event) {
  switch (event->button()) {
    case Qt::LeftButton:
      alignCamera();
      break;
    case Qt::RightButton:
      centerScene();
      break;
    case Qt::MiddleButton:
      showEntireScene();
      break;
    default:
      QOpenGLWidget::mouseDoubleClickEvent(event);
      return;
  }
  mouseAction_ = MouseAction::None;
  event->accept();
}

void QtmGLViewer::wheelEvent(QWheelEvent *event) {
  const float wheelSensitivityCoef = 8e-4f;  // as in libQGLViewer
  camera_.zoom(float(event->angleDelta().y()) * wheelSensitivity_ *
               wheelSensitivityCoef);
  update();
  event->accept();
}

void QtmGLViewer::keyPressEvent(QKeyEvent *event) {
  if (event->modifiers() != Qt::NoModifier) {
    QOpenGLWidget::keyPressEvent(event);
    return;
  }
  switch (event->key()) {
    case Qt::Key_A:
      toggleAxisIsDrawn();
      break;
    case Qt::Key_G:
      toggleGridIsDrawn();
      break;
    case Qt::Key_F:
      toggleFPSIsDisplayed();
      break;
    case Qt::Key_O:
      toggleCameraType();
      break;
    default:
      QOpenGLWidget::keyPressEvent(event);
      return;
  }
  event->accept();
}

void QtmGLViewer::spinStep() {
  camera_.rotateAroundPivot(spinningRotation_);
  update();
}
