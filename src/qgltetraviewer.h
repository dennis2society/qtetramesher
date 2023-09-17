/*
 * qgltetraviewer.h
 *
 *  Created on: Sep 10, 2011
 *      Author: Dennis Luebke
 */

#ifndef Q_TETRA_VIEWER_H_
#define Q_TETRA_VIEWER_H_

#include <QGLViewer/qglviewer.h>
#include "QGLTetraMesh.h"

class QGLTetraViewer : public QGLViewer
{

    Q_OBJECT

public :
#if QT_VERSION < 0x040000
  QGLTetraViewer(QWidget *parent, const char *name);
#else
  QGLTetraViewer(QWidget *parent);
#endif

  QGLTetraMesh* tMesh;

  void generateOctree(int d);
  float getMaxBBox() const;

  public slots:
	void ToggleTetraVis(int i);
	void ToggleTriangleVis(int i);
    void ToggleOctreeVis(int i);
	void ToggleBBox(int i);
	void SetCutPlane(int i);
    void SetZRange(int lower, int upper);
	void ShowStatusMessage(const QString& msg_, int duration_ = 2000);
    void loadSurface(const QString& fileName_ = "");
    void loadGMSH(const QString& fileName_ = "");
    void saveGMSH();
    void saveTetgen();
    void saveSurface();
    void selectSurfaceColor();
    void selectSurfaceColorWireframe();
    void selectTetraColor();
    void selectTetraColorWireframe();
    void scale10();
    void scale01();

signals:
    void onLoad();

protected :
    virtual void draw();
    virtual QString helpString() const;

    float _maxBBox; // maximum value in BBox lengths to determine suggested tetra size
};

#endif /* Q_TETRA_VIEWER_H_ */
