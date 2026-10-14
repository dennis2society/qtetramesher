/*
 * Camera.cpp
 *
 * Camera math follows libQGLViewer (Camera, ManipulatedCameraFrame) so that
 * the interaction feels the same.
 */

#include "QtmGLViewer/Camera.hpp"

#include <QtMath>
#include <algorithm>
#include <cmath>

namespace qtm {

namespace {

// libQGLViewer's deformed ball: a sphere that smoothly turns into a
// hyperbolic sheet away from the center.
float projectOnBall(float x, float y) {
  const float size = 1.0f;
  const float size2 = size * size;
  const float sizeLimit = size2 * 0.5f;
  const float d = x * x + y * y;
  return d < sizeLimit ? std::sqrt(size2 - d) : sizeLimit / std::sqrt(d);
}

}  // namespace

Camera::Camera()
    : fieldOfView_(float(M_PI) / 4.0f), zClippingCoef_(std::sqrt(3.0f)) {
  showEntireScene();
}

void Camera::setSceneCenter(const QVector3D &center) {
  // NaN or infinite values would poison all matrices
  if (!std::isfinite(center.x()) || !std::isfinite(center.y()) ||
      !std::isfinite(center.z()))
    return;
  sceneCenter_ = center;
  pivot_ = center;
}

void Camera::setSceneRadius(float radius) {
  if (!std::isfinite(radius) || radius <= 0.0f) return;
  sceneRadius_ = radius;
}

void Camera::setOrientation(const QQuaternion &orientation) {
  orientation_ = orientation.normalized();
}

QVector3D Camera::viewDirection() const {
  return orientation_.rotatedVector(QVector3D(0.0f, 0.0f, -1.0f));
}

QVector3D Camera::upVector() const {
  return orientation_.rotatedVector(QVector3D(0.0f, 1.0f, 0.0f));
}

QVector3D Camera::rightVector() const {
  return orientation_.rotatedVector(QVector3D(1.0f, 0.0f, 0.0f));
}

float Camera::horizontalFieldOfView() const {
  return 2.0f * std::atan(std::tan(fieldOfView_ / 2.0f) * aspectRatio());
}

void Camera::setScreenSize(int width, int height) {
  screenWidth_ = std::max(width, 1);
  screenHeight_ = std::max(height, 1);
}

float Camera::aspectRatio() const {
  return float(screenWidth_) / float(screenHeight_);
}

float Camera::zNear() const {
  const float zNearScene = zClippingCoef_ * sceneRadius_;
  float z = distanceToSceneCenter() - zNearScene;
  // prevents negative or null zNear values
  const float zMin = zNearCoef_ * zNearScene;
  if (z < zMin) z = (type_ == Type::Perspective) ? zMin : 0.0f;
  return z;
}

float Camera::zFar() const {
  return distanceToSceneCenter() + zClippingCoef_ * sceneRadius_;
}

void Camera::getOrthoWidthHeight(float &halfWidth, float &halfHeight) const {
  const float orthoCoef = std::tan(fieldOfView_ / 2.0f);
  const float dist = orthoCoef * std::fabs(cameraCoordinatesOf(pivot_).z());
  const float aspect = aspectRatio();
  halfWidth = dist * ((aspect < 1.0f) ? 1.0f : aspect);
  halfHeight = dist * ((aspect < 1.0f) ? 1.0f / aspect : 1.0f);
}

QMatrix4x4 Camera::viewMatrix() const {
  QMatrix4x4 m;
  m.rotate(orientation_.conjugated());
  m.translate(-position_);
  return m;
}

QMatrix4x4 Camera::projectionMatrix() const {
  QMatrix4x4 m;
  if (type_ == Type::Perspective) {
    m.perspective(qRadiansToDegrees(fieldOfView_), aspectRatio(), zNear(),
                  zFar());
  } else {
    float w, h;
    getOrthoWidthHeight(w, h);
    m.ortho(-w, w, -h, h, zNear(), zFar());
  }
  return m;
}

QVector3D Camera::cameraCoordinatesOf(const QVector3D &world) const {
  return orientation_.conjugated().rotatedVector(world - position_);
}

QVector3D Camera::worldCoordinatesOf(const QVector3D &local) const {
  return orientation_.rotatedVector(local) + position_;
}

QVector3D Camera::projectedCoordinatesOf(const QVector3D &world) const {
  const QVector4D clip =
      projectionMatrix() * viewMatrix() * QVector4D(world, 1.0f);
  if (qFuzzyIsNull(clip.w())) return QVector3D();
  const QVector3D ndc = clip.toVector3D() / clip.w();
  return QVector3D((ndc.x() + 1.0f) * 0.5f * screenWidth_,
                   (1.0f - ndc.y()) * 0.5f * screenHeight_,
                   (ndc.z() + 1.0f) * 0.5f);
}

float Camera::distanceToSceneCenter() const {
  return std::fabs(cameraCoordinatesOf(sceneCenter_).z());
}

void Camera::showEntireScene() { fitSphere(sceneCenter_, sceneRadius_); }

void Camera::fitSphere(const QVector3D &center, float radius) {
  float distance = 0.0f;
  if (type_ == Type::Perspective) {
    const float yview = radius / std::sin(fieldOfView_ / 2.0f);
    const float xview = radius / std::sin(horizontalFieldOfView() / 2.0f);
    distance = std::max(xview, yview);
  } else {
    const float orthoCoef = std::tan(fieldOfView_ / 2.0f);
    distance = QVector3D::dotProduct(center - pivot_, viewDirection()) +
               radius / orthoCoef;
  }
  position_ = center - distance * viewDirection();
}

void Camera::centerScene() {
  // project the camera position onto the line through the scene center
  // along the view direction
  const QVector3D dir = viewDirection();
  position_ = sceneCenter_ +
              dir * QVector3D::dotProduct(position_ - sceneCenter_, dir);
}

void Camera::alignWithNearestAxis() {
  const QVector3D axes[3] = {QVector3D(1, 0, 0), QVector3D(0, 1, 0),
                             QVector3D(0, 0, 1)};
  auto snap = [&axes](const QVector3D &v, int skipAxis, int &chosenAxis) {
    int best = -1;
    float bestValue = -1.0f;
    for (int i = 0; i < 3; ++i) {
      if (i == skipAxis) continue;
      const float value = std::fabs(QVector3D::dotProduct(v, axes[i]));
      if (value > bestValue) {
        bestValue = value;
        best = i;
      }
    }
    chosenAxis = best;
    return QVector3D::dotProduct(v, axes[best]) < 0.0f ? -axes[best]
                                                        : axes[best];
  };
  int xAxis, yAxis;
  const QVector3D newX = snap(rightVector(), -1, xAxis);
  const QVector3D newY = snap(upVector(), xAxis, yAxis);
  const QVector3D newZ = QVector3D::crossProduct(newX, newY);
  const QQuaternion newOrientation = QQuaternion::fromAxes(newX, newY, newZ);
  // rotate around the pivot so it keeps its position relative to the camera
  const QVector3D local = cameraCoordinatesOf(pivot_);
  orientation_ = newOrientation.normalized();
  position_ = pivot_ - orientation_.rotatedVector(local);
}

void Camera::rotateAroundPivot(const QQuaternion &localRotation) {
  const QQuaternion worldRotation =
      orientation_ * localRotation * orientation_.conjugated();
  orientation_ = (worldRotation * orientation_).normalized();
  position_ = pivot_ + worldRotation.rotatedVector(position_ - pivot_);
}

void Camera::translate(const QVector3D &localTranslation) {
  position_ += orientation_.rotatedVector(localTranslation);
}

QQuaternion Camera::trackballRotation(const QPointF &prevPos,
                                      const QPointF &pos,
                                      float sensitivity) const {
  const QVector3D center = projectedCoordinatesOf(pivot_);
  const float cx = center.x();
  const float cy = center.y();
  const float px = sensitivity * (float(prevPos.x()) - cx) / screenWidth_;
  const float py = sensitivity * (cy - float(prevPos.y())) / screenHeight_;
  const float dx = sensitivity * (float(pos.x()) - cx) / screenWidth_;
  const float dy = sensitivity * (cy - float(pos.y())) / screenHeight_;

  const QVector3D p1(px, py, projectOnBall(px, py));
  const QVector3D p2(dx, dy, projectOnBall(dx, dy));
  const QVector3D axis = QVector3D::crossProduct(p2, p1);
  const float axisLength2 = axis.lengthSquared();
  if (axisLength2 < 1e-12f) return QQuaternion();
  const float s = std::sqrt(axisLength2 / p1.lengthSquared() /
                            p2.lengthSquared());
  const float angle = 5.0f * std::asin(std::min(s, 1.0f));
  return QQuaternion::fromAxisAndAngle(axis.normalized(),
                                       qRadiansToDegrees(angle));
}

QVector3D Camera::panTranslation(const QPointF &delta) const {
  QVector3D trans(float(delta.x()), -float(delta.y()), 0.0f);
  if (type_ == Type::Perspective) {
    trans *= 2.0f * std::tan(fieldOfView_ / 2.0f) *
             std::fabs(cameraCoordinatesOf(pivot_).z()) / screenHeight_;
  } else {
    float w, h;
    getOrthoWidthHeight(w, h);
    trans.setX(trans.x() * 2.0f * w / screenWidth_);
    trans.setY(trans.y() * 2.0f * h / screenHeight_);
  }
  return trans;
}

void Camera::zoom(float delta) {
  const float coef = std::max(std::fabs(cameraCoordinatesOf(pivot_).z()),
                              0.2f * sceneRadius_);
  translate(QVector3D(0.0f, 0.0f, -coef * delta));
}

}  // namespace qtm
