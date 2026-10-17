/*
 * MeshBuffer.cpp
 */

#include "QtmGLViewer/MeshBuffer.hpp"

#include <QOpenGLContext>
#include <QOpenGLExtraFunctions>

#include "QtmGLViewer/MeshData.hpp"

namespace qtm {

namespace {

QOpenGLExtraFunctions *glFunctions() {
  return QOpenGLContext::currentContext()->extraFunctions();
}

}  // namespace

MeshBuffer::MeshBuffer() : indexBuffer_(QOpenGLBuffer::IndexBuffer) {}

MeshBuffer::~MeshBuffer() { destroy(); }

void MeshBuffer::ensureCreated() {
  if (vao_) return;
  vao_ = std::make_unique<QOpenGLVertexArrayObject>();
  vao_->create();
}

void MeshBuffer::upload(const MeshData &mesh, GLenum primitive) {
  setPrimitive(primitive);
  setAttribute(Position, reinterpret_cast<const float *>(mesh.positions.data()),
               int(mesh.positions.size()));
  if (mesh.normals.size() == mesh.positions.size() && !mesh.normals.empty())
    setAttribute(Normal, reinterpret_cast<const float *>(mesh.normals.data()),
                 int(mesh.normals.size()));
  else
    removeAttribute(Normal);
  if (mesh.colors.size() == mesh.positions.size() && !mesh.colors.empty())
    setAttribute(Color, reinterpret_cast<const float *>(mesh.colors.data()),
                 int(mesh.colors.size()));
  else
    removeAttribute(Color);
  if (mesh.indices.empty())
    clearIndices();
  else
    setIndices(mesh.indices.data(), int(mesh.indices.size()));
}

void MeshBuffer::setAttribute(GLuint location, const float *data, int count,
                              int components, int divisor) {
  ensureCreated();
  QOpenGLExtraFunctions *gl = glFunctions();
  auto it = attributes_.find(location);
  if (it == attributes_.end()) {
    QOpenGLBuffer buffer(QOpenGLBuffer::VertexBuffer);
    buffer.create();
    buffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    it = attributes_.emplace(location, buffer).first;
  }
  vao_->bind();
  it->second.bind();
  it->second.allocate(data, int(count * components * sizeof(float)));
  gl->glEnableVertexAttribArray(location);
  gl->glVertexAttribPointer(location, components, GL_FLOAT, GL_FALSE, 0,
                            nullptr);
  gl->glVertexAttribDivisor(location, GLuint(divisor));
  it->second.release();
  vao_->release();
  if (location == Position && divisor == 0) vertexCount_ = count;
}

void MeshBuffer::removeAttribute(GLuint location) {
  auto it = attributes_.find(location);
  if (it == attributes_.end()) return;
  vao_->bind();
  glFunctions()->glDisableVertexAttribArray(location);
  vao_->release();
  it->second.destroy();
  attributes_.erase(it);
}

bool MeshBuffer::hasAttribute(GLuint location) const {
  return attributes_.count(location) > 0;
}

void MeshBuffer::setIndices(const uint32_t *data, int count) {
  ensureCreated();
  vao_->bind();
  if (!indexBuffer_.isCreated()) {
    indexBuffer_.create();
    indexBuffer_.setUsagePattern(QOpenGLBuffer::StaticDraw);
  }
  // the element buffer binding is VAO state, so keep it bound
  indexBuffer_.bind();
  indexBuffer_.allocate(data, int(count * sizeof(uint32_t)));
  vao_->release();
  indexCount_ = count;
}

void MeshBuffer::clearIndices() {
  if (indexBuffer_.isCreated()) indexBuffer_.destroy();
  indexCount_ = 0;
}

bool MeshBuffer::isEmpty() const {
  if (!vao_ || !hasAttribute(Position)) return true;
  return indexBuffer_.isCreated() ? indexCount_ == 0 : vertexCount_ == 0;
}

void MeshBuffer::draw() const {
  if (isEmpty()) return;
  QOpenGLExtraFunctions *gl = glFunctions();
  vao_->bind();
  if (indexBuffer_.isCreated()) {
    if (instanceCount_ > 0)
      gl->glDrawElementsInstanced(primitive_, indexCount_, GL_UNSIGNED_INT,
                                  nullptr, instanceCount_);
    else
      gl->glDrawElements(primitive_, indexCount_, GL_UNSIGNED_INT, nullptr);
  } else {
    if (instanceCount_ > 0)
      gl->glDrawArraysInstanced(primitive_, 0, vertexCount_, instanceCount_);
    else
      gl->glDrawArrays(primitive_, 0, vertexCount_);
  }
  vao_->release();
}

void MeshBuffer::destroy() {
  for (auto &entry : attributes_) entry.second.destroy();
  attributes_.clear();
  if (indexBuffer_.isCreated()) indexBuffer_.destroy();
  vao_.reset();
  vertexCount_ = 0;
  indexCount_ = 0;
  instanceCount_ = 0;
}

}  // namespace qtm
