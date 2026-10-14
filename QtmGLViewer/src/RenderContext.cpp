/*
 * RenderContext.cpp
 */

#include "QtmGLViewer/RenderContext.hpp"

#include <QOpenGLContext>
#include <QOpenGLExtraFunctions>
#include <QtDebug>

#include "QtmGLViewer/Camera.hpp"
#include "QtmGLViewer/MeshBuffer.hpp"

namespace qtm {

namespace {

// Directional light along the eye space view axis with a non-local viewer,
// like the fixed function default GL_LIGHT0 used by libQGLViewer. The 0.2
// ambient term matches the default GL_LIGHT_MODEL_AMBIENT.
const char *kLightingGlsl = R"(
vec3 qtmHeadlight(vec3 eyeNormal, vec3 baseColor, float specular,
                  float shininess) {
  vec3 n = normalize(eyeNormal);
  const vec3 lightDir = vec3(0.0, 0.0, 1.0);
  const vec3 halfVector = vec3(0.0, 0.0, 1.0);
  float diffuse = max(dot(n, lightDir), 0.0);
  float spec = diffuse > 0.0 ? pow(max(dot(n, halfVector), 0.0), shininess)
                             : 0.0;
  return 0.2 * baseColor + diffuse * baseColor + specular * vec3(spec);
}
)";

const char *kSolidVertexShader = R"(#version 330 core
layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec3 a_color;
uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;
uniform mat3 u_normalMatrix;
uniform vec3 u_color;
uniform bool u_useVertexColor;
out vec3 v_normal;
out vec3 v_color;
void main() {
  v_normal = u_normalMatrix * a_normal;
  v_color = u_useVertexColor ? a_color : u_color;
  gl_Position = u_projection * u_view * u_model * vec4(a_position, 1.0);
}
)";

const char *kSolidFragmentShader = R"(
in vec3 v_normal;
in vec3 v_color;
uniform float u_specular;
uniform float u_shininess;
out vec4 fragColor;
void main() {
  // two-sided lighting: back faces use the flipped normal
  vec3 n = gl_FrontFacing ? v_normal : -v_normal;
  fragColor = vec4(qtmHeadlight(n, v_color, u_specular, u_shininess), 1.0);
}
)";

const char *kLineVertexShader = R"(#version 330 core
layout(location = 0) in vec3 a_position;
layout(location = 2) in vec3 a_color;
uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;
uniform vec3 u_color;
uniform bool u_useVertexColor;
out vec3 v_color;
void main() {
  v_color = u_useVertexColor ? a_color : u_color;
  gl_Position = u_projection * u_view * u_model * vec4(a_position, 1.0);
}
)";

const char *kLineFragmentShader = R"(#version 330 core
in vec3 v_color;
out vec4 fragColor;
void main() {
  fragColor = vec4(v_color, 1.0);
}
)";

std::unique_ptr<QOpenGLShaderProgram> buildProgram(const QString &name,
                                                   const QString &vertex,
                                                   const QString &fragment) {
  auto program = std::make_unique<QOpenGLShaderProgram>();
  if (!program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertex) ||
      !program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragment) ||
      !program->link()) {
    qWarning() << "QtmGLViewer: failed to build" << name
               << "shader:" << program->log();
    return nullptr;
  }
  return program;
}

QVector3D toVector(const QColor &c) {
  return QVector3D(float(c.redF()), float(c.greenF()), float(c.blueF()));
}

}  // namespace

RenderContext::RenderContext() = default;

RenderContext::~RenderContext() = default;

bool RenderContext::initialize() {
  const QString solidFragment = QStringLiteral("#version 330 core\n") +
                                QString::fromLatin1(kLightingGlsl) +
                                QString::fromLatin1(kSolidFragmentShader);
  solidProgram_ = buildProgram(QStringLiteral("solid"),
                               QString::fromLatin1(kSolidVertexShader),
                               solidFragment);
  lineProgram_ = buildProgram(QStringLiteral("line"),
                              QString::fromLatin1(kLineVertexShader),
                              QString::fromLatin1(kLineFragmentShader));
  valid_ = solidProgram_ && lineProgram_;
  return valid_;
}

void RenderContext::beginFrame(const Camera &camera) {
  camera_ = &camera;
  view_ = camera.viewMatrix();
  projection_ = camera.projectionMatrix();
}

QOpenGLExtraFunctions *RenderContext::gl() const {
  return QOpenGLContext::currentContext()->extraFunctions();
}

const char *RenderContext::lightingGlsl() { return kLightingGlsl; }

void RenderContext::setMatrixUniforms(QOpenGLShaderProgram &program,
                                      const QMatrix4x4 &model) const {
  program.setUniformValue("u_model", model);
  program.setUniformValue("u_view", view_);
  program.setUniformValue("u_projection", projection_);
  program.setUniformValue("u_normalMatrix", (view_ * model).normalMatrix());
}

void RenderContext::drawSolid(const MeshBuffer &mesh, const QColor &color,
                              const QMatrix4x4 &model, float specular,
                              float shininess) const {
  if (!valid_ || mesh.isEmpty()) return;
  QOpenGLShaderProgram &program = *solidProgram_;
  program.bind();
  setMatrixUniforms(program, model);
  program.setUniformValue("u_color", toVector(color));
  program.setUniformValue("u_useVertexColor",
                          mesh.hasAttribute(MeshBuffer::Color));
  program.setUniformValue("u_specular", specular);
  program.setUniformValue("u_shininess", shininess);
  mesh.draw();
  program.release();
}

void RenderContext::drawLines(const MeshBuffer &mesh, const QColor &color,
                              const QMatrix4x4 &model) const {
  if (!valid_ || mesh.isEmpty()) return;
  QOpenGLShaderProgram &program = *lineProgram_;
  program.bind();
  program.setUniformValue("u_model", model);
  program.setUniformValue("u_view", view_);
  program.setUniformValue("u_projection", projection_);
  program.setUniformValue("u_color", toVector(color));
  program.setUniformValue("u_useVertexColor",
                          mesh.hasAttribute(MeshBuffer::Color));
  mesh.draw();
  program.release();
}

}  // namespace qtm
