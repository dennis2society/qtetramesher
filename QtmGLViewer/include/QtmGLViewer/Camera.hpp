/*
 * Camera.hpp
 *
 * Trackball camera modelled after the libQGLViewer camera: a frame
 * (position + orientation) that revolves around a pivot point, plus a
 * scene center/radius used for automatic clipping planes and fitting.
 */

#ifndef QTMGLVIEWER_CAMERA_HPP_
#define QTMGLVIEWER_CAMERA_HPP_

#include <QMatrix4x4>
#include <QPointF>
#include <QQuaternion>
#include <QVector3D>

namespace qtm {

class Camera {
 public:
  enum class Type { Perspective, Orthographic };

  Camera();

  // Scene description, used for clipping planes, grid/axis size and fitting.
  // setSceneCenter() also moves the pivot point to the new center.
  void setSceneCenter(const QVector3D &center);
  QVector3D sceneCenter() const { return sceneCenter_; }
  void setSceneRadius(float radius);
  float sceneRadius() const { return sceneRadius_; }

  // The point the camera revolves around.
  void setPivotPoint(const QVector3D &pivot) { pivot_ = pivot; }
  QVector3D pivotPoint() const { return pivot_; }

  // Camera frame in world coordinates. The camera looks along its local -Z
  // axis with local +Y as up vector.
  void setPosition(const QVector3D &position) { position_ = position; }
  QVector3D position() const { return position_; }
  void setOrientation(const QQuaternion &orientation);
  QQuaternion orientation() const { return orientation_; }
  QVector3D viewDirection() const;
  QVector3D upVector() const;
  QVector3D rightVector() const;

  // Projection
  void setType(Type type) { type_ = type; }
  Type type() const { return type_; }
  // vertical field of view in radians (default: pi/4)
  void setFieldOfView(float fov) { fieldOfView_ = fov; }
  float fieldOfView() const { return fieldOfView_; }
  float horizontalFieldOfView() const;
  void setScreenSize(int width, int height);
  int screenWidth() const { return screenWidth_; }
  int screenHeight() const { return screenHeight_; }
  float aspectRatio() const;
  void setZNearCoefficient(float coef) { zNearCoef_ = coef; }
  float zNearCoefficient() const { return zNearCoef_; }
  void setZClippingCoefficient(float coef) { zClippingCoef_ = coef; }
  float zClippingCoefficient() const { return zClippingCoef_; }
  float zNear() const;
  float zFar() const;
  void getOrthoWidthHeight(float &halfWidth, float &halfHeight) const;

  QMatrix4x4 viewMatrix() const;
  QMatrix4x4 projectionMatrix() const;

  // Coordinate conversions
  QVector3D cameraCoordinatesOf(const QVector3D &world) const;
  QVector3D worldCoordinatesOf(const QVector3D &local) const;
  // Returns window coordinates (logical pixels, y pointing down) in x/y and
  // the depth in [0, 1] in z.
  QVector3D projectedCoordinatesOf(const QVector3D &world) const;
  // Distance along the view direction from the camera to the scene center.
  float distanceToSceneCenter() const;

  // Camera motion
  void showEntireScene();
  void fitSphere(const QVector3D &center, float radius);
  // Moves the camera sideways so that the scene center is in the middle of
  // the screen.
  void centerScene();
  // Snaps the camera axes to the nearest world axes (rotating around pivot).
  void alignWithNearestAxis();
  // Rotates the camera around the pivot point. The rotation is given in
  // camera (local) coordinates.
  void rotateAroundPivot(const QQuaternion &localRotation);
  // Translation in camera (local) coordinates.
  void translate(const QVector3D &localTranslation);

  // Mouse helpers (positions in logical pixels)
  // Deformed-ball trackball rotation between two mouse positions, expressed
  // in camera coordinates.
  QQuaternion trackballRotation(const QPointF &prevPos, const QPointF &pos,
                                float sensitivity = 1.0f) const;
  // Local translation that makes the scene (at pivot depth) follow a mouse
  // displacement of delta = prevPos - pos.
  QVector3D panTranslation(const QPointF &delta) const;
  // Moves the camera along its view direction. Positive values zoom in.
  void zoom(float delta);

 private:
  QVector3D position_;
  QQuaternion orientation_;
  QVector3D pivot_;
  QVector3D sceneCenter_;
  float sceneRadius_ = 1.0f;
  Type type_ = Type::Perspective;
  float fieldOfView_;
  int screenWidth_ = 600;
  int screenHeight_ = 400;
  float zNearCoef_ = 0.005f;
  float zClippingCoef_;
};

}  // namespace qtm

#endif /* QTMGLVIEWER_CAMERA_HPP_ */
