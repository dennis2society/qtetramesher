/*
 * SphereViewer.hpp
 *
 * QtmGLViewer test subclass drawing a solid icosphere with optional
 * wireframe overlay.
 */

#ifndef QTMGLVIEWER_TEST_SPHEREVIEWER_HPP_
#define QTMGLVIEWER_TEST_SPHEREVIEWER_HPP_

#include <QtmGLViewer/QtmGLViewer.hpp>

class SphereViewer : public QtmGLViewer {
  Q_OBJECT

 public:
  explicit SphereViewer(QWidget *parent = nullptr);
  ~SphereViewer() override;

  float sphereRadius() const { return radius_; }

 public slots:
  void setSphereRadius(double radius);
  void setSubdivisions(int subdivisions);
  void setWireframeDrawn(bool drawn);
  void setSphereColor(const QColor &color);

 protected:
  void initializeScene() override;
  void cleanupScene() override;
  void draw(const qtm::RenderContext &context) override;

 private:
  void rebuildSphere();

  qtm::MeshBuffer sphere_;
  qtm::MeshBuffer edges_;
  float radius_ = 1.0f;
  int subdivisions_ = 3;
  int triangleCount_ = 0;
  int vertexCount_ = 0;
  bool wireframeDrawn_ = false;
  bool dirty_ = true;
  QColor color_{204, 204, 217};
};

#endif /* QTMGLVIEWER_TEST_SPHEREVIEWER_HPP_ */
