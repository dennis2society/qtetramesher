/*
 * MeshBuffer.hpp
 *
 * GPU side geometry: one VAO with one VBO per vertex attribute and an
 * optional index buffer. All methods (including the destructor) require the
 * OpenGL context the buffer was created in to be current.
 */

#ifndef QTMGLVIEWER_MESHBUFFER_HPP_
#define QTMGLVIEWER_MESHBUFFER_HPP_

#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QtGui/qopengl.h>
#include <cstdint>
#include <map>
#include <memory>

namespace qtm {

struct MeshData;

class MeshBuffer {
 public:
  // Attribute locations used by the built-in shaders.
  enum Location : GLuint { Position = 0, Normal = 1, Color = 2 };

  MeshBuffer();
  ~MeshBuffer();
  MeshBuffer(const MeshBuffer &) = delete;
  MeshBuffer &operator=(const MeshBuffer &) = delete;

  // Uploads a MeshData (positions + optional normals, colors, indices).
  void upload(const MeshData &mesh, GLenum primitive);

  // Generic float attribute upload. count is the number of elements (e.g.
  // vertices), each with the given number of float components. A divisor > 0
  // makes it a per-instance attribute. Uploading Position with divisor 0
  // sets the vertex count used by non-indexed draws.
  void setAttribute(GLuint location, const float *data, int count,
                    int components = 3, int divisor = 0);
  void removeAttribute(GLuint location);
  bool hasAttribute(GLuint location) const;

  void setIndices(const uint32_t *data, int count);
  void clearIndices();

  void setPrimitive(GLenum primitive) { primitive_ = primitive; }
  GLenum primitive() const { return primitive_; }
  // Vertex count for non-indexed draws (set automatically from Position).
  void setVertexCount(int count) { vertexCount_ = count; }
  int vertexCount() const { return vertexCount_; }
  int indexCount() const { return indexCount_; }
  // > 0 enables instanced drawing
  void setInstanceCount(int count) { instanceCount_ = count; }
  int instanceCount() const { return instanceCount_; }

  bool isEmpty() const;
  void draw() const;
  void destroy();

 private:
  void ensureCreated();

  std::unique_ptr<QOpenGLVertexArrayObject> vao_;
  std::map<GLuint, QOpenGLBuffer> attributes_;
  QOpenGLBuffer indexBuffer_;
  GLenum primitive_ = GL_TRIANGLES;
  int vertexCount_ = 0;
  int indexCount_ = 0;
  int instanceCount_ = 0;
};

}  // namespace qtm

#endif /* QTMGLVIEWER_MESHBUFFER_HPP_ */
