/*
 * MobiusViewer.hpp
 *
 * QtmGLViewer test subclass drawing a solid Moebius strip with optional
 * wireframe overlay.
 */

#ifndef QTMGLVIEWER_TEST_MOBIUSVIEWER_HPP_
#define QTMGLVIEWER_TEST_MOBIUSVIEWER_HPP_

#include <QtmGLViewer/QtmGLViewer.hpp>

class MobiusViewer : public QtmGLViewer {
  Q_OBJECT

 public:
  explicit MobiusViewer(QWidget *parent = nullptr);
  ~MobiusViewer() override;

  float stripRadius() const { return radius_; }

 public slots:
  void setStripRadius(double radius);
  void setWireframeDrawn(bool drawn);

 protected:
  void initializeScene() override;
  void cleanupScene() override;
  void draw(const qtm::RenderContext &context) override;

 private:
  void rebuildStrip();

  qtm::MeshBuffer strip_;
  qtm::MeshBuffer edges_;
  float radius_ = 0.8f;
  int triangleCount_ = 0;
  int vertexCount_ = 0;
  bool wireframeDrawn_ = false;
  bool dirty_ = true;
};

#endif /* QTMGLVIEWER_TEST_MOBIUSVIEWER_HPP_ */
