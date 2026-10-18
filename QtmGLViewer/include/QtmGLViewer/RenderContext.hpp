/*
 * RenderContext.hpp
 *
 * Per-frame rendering state handed to QtmGLViewer::draw(): camera matrices,
 * OpenGL functions and the built-in shaders (lit solids and flat lines).
 */

#ifndef QTMGLVIEWER_RENDERCONTEXT_HPP_
#define QTMGLVIEWER_RENDERCONTEXT_HPP_

#include <QColor>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include <memory>

class QOpenGLExtraFunctions;

namespace qtm {

class Camera;
class MeshBuffer;

class RenderContext {
 public:
  RenderContext();
  ~RenderContext();

  // Compiles the built-in shaders. Requires a current OpenGL context.
  bool initialize();
  bool isValid() const { return valid_; }
  // Updates the matrices from the camera. Called by QtmGLViewer every frame.
  void beginFrame(const Camera &camera);

  QOpenGLExtraFunctions *gl() const;
  const Camera &camera() const { return *camera_; }
  const QMatrix4x4 &viewMatrix() const { return view_; }
  const QMatrix4x4 &projectionMatrix() const { return projection_; }
  QMatrix4x4 viewProjectionMatrix() const { return projection_ * view_; }

  // Draws a mesh with normals using a camera headlight (Blinn-Phong). Uses
  // the per-vertex colors if the mesh has a Color attribute.
  void drawSolid(const MeshBuffer &mesh, const QColor &color,
                 const QMatrix4x4 &model = QMatrix4x4(),
                 float specular = 0.6f, float shininess = 50.0f) const;
  // Draws a mesh unlit (lines, points or triangles). Uses the per-vertex
  // colors if the mesh has a Color attribute.
  void drawLines(const MeshBuffer &mesh, const QColor &color,
                 const QMatrix4x4 &model = QMatrix4x4()) const;

  // Helpers for custom shaders:
  // GLSL function "vec3 qtmHeadlight(vec3 eyeNormal, vec3 baseColor,
  // float specular, float shininess)" implementing the built-in lighting.
  static const char *lightingGlsl();
  // Sets u_model, u_view, u_projection and u_normalMatrix (mat3, eye space).
  void setMatrixUniforms(QOpenGLShaderProgram &program,
                         const QMatrix4x4 &model = QMatrix4x4()) const;

 private:
  std::unique_ptr<QOpenGLShaderProgram> solidProgram_;
  std::unique_ptr<QOpenGLShaderProgram> lineProgram_;
  const Camera *camera_ = nullptr;
  QMatrix4x4 view_;
  QMatrix4x4 projection_;
  bool valid_ = false;
};

}  // namespace qtm

#endif /* QTMGLVIEWER_RENDERCONTEXT_HPP_ */
