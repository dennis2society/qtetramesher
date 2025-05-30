/*
 * qgltetraviewer.cpp
 *
 * Derived from QGLViewer to allow custom mesh drawing.
 *
 *  Created 2012
 *      Author: Dennis Luebke
 */

#include "qgltetraviewer.hpp"

#include <math.h>
#include <qtetramesher_version.h>

#include <QColorDialog>
#include <QDateTime>
#include <QFileDialog>
#include <sstream>
#include <string>

// Constructor must call the base class constructor.
QGLTetraViewer::QGLTetraViewer(QWidget *parent) : QGLViewer(parent) {
  tMesh = new QGLTetraMesh();
}

float QGLTetraViewer::getMaxBBox() const { return _maxBBox; }

void QGLTetraViewer::draw() {
  if (tMesh->IsReady()) {
    BoundingBox bb = tMesh->GetBoundingBox();
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    tMesh->Draw();
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    std::stringstream ss;
    std::string stdMSG;
    QString msg;
    if (tMesh->GetSurface() != NULL) {
      ss << "Surface Mesh: "
         << "Vertices: " << tMesh->GetSurface()->GetVertices().size()
         << "; Triangles: " << tMesh->GetSurface()->GetTriangles().size();
      stdMSG = ss.str();
      msg = QString::fromStdString(stdMSG);
      glColor3f(0.1f, 0.8f, 0.1f);
      drawText(10, 30, msg);
      ss.str("");
      _maxBBox = bb.max.x - bb.min.x;
      if (_maxBBox < (bb.max.y - bb.min.y)) _maxBBox = bb.max.y - bb.min.y;
      if (_maxBBox < (bb.max.z - bb.min.z)) _maxBBox = bb.max.z - bb.min.z;
      ss << "BBox Min:" << bb.min.x << "/" << bb.min.y << "/" << bb.min.z
         << "; BBox Max: " << bb.max.x << "/" << bb.max.y << "/" << bb.max.z
         << "; Max Value: " << _maxBBox;
      stdMSG = ss.str();
      msg = QString::fromStdString(stdMSG);
      glColor3f(0.1f, 0.8f, 0.1f);
      drawText(10, 45, msg);
      ss.str("");
    }
    if (tMesh->GetTetraMesh() != NULL) {
      glDisable(GL_LIGHTING);
      glDisable(GL_DEPTH_TEST);
      ss << "Tetra Mesh: "
         << "Vertices: " << tMesh->GetTetraMesh()->GetVertices().size()
         << "; Tetras: " << tMesh->GetTetraMesh()->GetTetrahedra().size();
      stdMSG = ss.str();
      msg = QString::fromStdString(stdMSG);
      glColor3f(0.8f, 0.8f, 0.1f);
      drawText(10, 60, msg);
      ss.str("");
      ss << "Tetra Mesh: "
         << "Triangles: " << tMesh->GetTetraMesh()->GetTriangles().size()
         << "; Edges: " << tMesh->GetTetraMesh()->GetEdges().size();
      stdMSG = ss.str();
      msg = QString::fromStdString(stdMSG);
      glColor3f(0.8f, 0.8f, 0.1f);
      drawText(10, 75, msg);
    }
  } else {
    //// Draws a spiral
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    const float nbSteps = 200.0;
    const uint nb = static_cast<uint>(nbSteps);
    glBegin(GL_QUAD_STRIP);
    for (auto i = 0; i < nb; ++i) {
      float ratio = i / nbSteps;
      float angle = 21.0 * ratio;
      float c = cos(angle);
      float s = sin(angle);
      float r1 = 1.0 - 0.8 * ratio;
      float r2 = 0.8 - 0.8 * ratio;
      float alt = ratio - 0.5;
      const float nor = .5;
      const float up = sqrt(1.0 - nor * nor);
      glColor3f(1.0 - ratio, 0.2f, ratio);
      glNormal3f(nor * c, up, nor * s);
      glVertex3f(r1 * c, alt, r1 * s);
      glVertex3f(r2 * c, alt + 0.05, r2 * s);
    }
    glEnd();
  }
  glEnable(GL_LIGHTING);
  glEnable(GL_DEPTH_TEST);
}

QString QGLTetraViewer::helpString() const {
  const QString version(PROJECT_VER);
  int year = QDate::currentDate().year();
  QString text("<h2>QTetraMesher</h2>");
  text += "Version: ";
  text += version;
  text += "<br>";
  text += "<br>";
  text += "&copy; 2013 - " + QString::number(year) +
          ", Dennis L&uuml;bke, qtm (at) dennis2society.de";
  text += "<br>";
  text += "<br>";
  text +=
      "This is a Qt-based program for Windows and Linux to generate "
      "tetrahedral meshes for "
      "finite element simulation from various surface mesh formats. It "
      "also offers a fast "
      "and easy-to-use mesh viewer based on QGLViewer and allows basic "
      "mesh manipulation.<br />"
      "<a href=\"https://github.com/dennis2society/qtetramesher\">"
      "https://github.com/dennis2society/qtetramesher</a><br /><br />"
      "Two different methods for tetrahedralization are possible:<br />"
      "Delaunay Triangulation (CGAL implementation)<br /> "
      "<a href=\"https://www.cgal.org\">https://www.cgal.org</a><br />"
      "and<br />"
      "Johnathan Shewchuk's Isosurface Stuffing algorithm<br />"
      "<a href=\"https://people.eecs.berkeley.edu/~jrs/papers/stuffing.pdf\">"
      "https://people.eecs.berkeley.edu/~jrs/papers/stuffing.pdf</a><br>"
      "(Implementation from Sofa Framework)<br />"
      "<a href=\"https://github.com/sofa-framework/sofa\">"
      "https://github.com/sofa-framework/sofa</a><br><br>";
  text +=
      "A second (unfortunately rather unstable) implementation of Shewchuk's "
      "Isosurface Stuffing (Quartet Tetrahedralize) made by "
      "Crawford Doran and Robert Bridson is available.<br />"
      "<a href=\"https://github.com/crawforddoran/quartet\">https://"
      "github.com/crawforddoran/quartet</a><br />";
  text += "<br />";
  text +=
      "Usage/build instructions available at <a "
      "href=\"http://qtm.dennis2society.de\">qtm.dennis2society.de</a>.<br />";
  text += "<br />";
  text +=
      "Sourcecode available at <a href=\"https://github.com/dennis2society/"
      "qtetramesher\">QTetraMesher at Github</a>.<br />";
  text += "<br />";
  text +=
      "<pre>This program is free software: you can redistribute it and/or "
      "modify it<br />"
      "under the terms of the GNU General Public License as published by the "
      "Free<br />"
      "Software Foundation, either version 3 of the License, or (at your "
      "option) any<br />"
      "later version.<br />"
      "This program is distributed in the hope that it will be useful,<br />"
      "but WITHOUT ANY WARRANTY; without even the implied warranty of<br />"
      "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the<br />"
      "GNU General Public License for more details.<br />"
      "You should have received a copy of the GNU General Public License<br />"
      "along with this program.  If not, see here: <a "
      "href=\"http://www.gnu.org/licenses/gpl.html\">GPL License</a>.";
  text += "<br>";
  return text;
}

void QGLTetraViewer::ToggleTetraVis(bool drawWireframe_, bool drawSolid_) {
  tMesh->ToggleTetraMesh(drawWireframe_, drawSolid_);
  // updateGL();
  update();
}

void QGLTetraViewer::ToggleTriangleVis(int i) {
  tMesh->ToggleTriangleMesh(i);
  update();
}

void QGLTetraViewer::ToggleOctreeVis(int i) {
  tMesh->ToggleOctreeVis(i);
  update();
}

void QGLTetraViewer::ToggleBBox(int i) {
  tMesh->ToggleBBox(i);
  update();
}

void QGLTetraViewer::ShowStatusMessage(const QString &msg_, int duration_) {
  displayMessage(msg_, duration_);
}

void QGLTetraViewer::scale10() {
  if (tMesh == NULL) {
    std::cerr << "Error! No mesh available..." << std::endl;
    return;
  }
  tMesh->scalex10();
  BoundingBox bb = tMesh->GetBoundingBox();
  _maxBBox = bb.max.x - bb.min.x;
  if (_maxBBox < (bb.max.y - bb.min.y)) _maxBBox = bb.max.y - bb.min.y;
  if (_maxBBox < (bb.max.z - bb.min.z)) _maxBBox = bb.max.z - bb.min.z;
  this->setSceneCenter(qglviewer::Vec(0, 0, 0));
  this->setSceneRadius(_maxBBox);
  update();
  // this->camera()->showEntireScene();
  // this->ShowStatusMessage("Mesh successfully scale by 10", 3000);
}

void QGLTetraViewer::scale01() {
  if (tMesh == NULL) {
    std::cerr << "Error! No mesh available..." << std::endl;
    return;
  }
  tMesh->scalex01();
  BoundingBox bb = tMesh->GetBoundingBox();
  _maxBBox = bb.max.x - bb.min.x;
  if (_maxBBox < (bb.max.y - bb.min.y)) _maxBBox = bb.max.y - bb.min.y;
  if (_maxBBox < (bb.max.z - bb.min.z)) _maxBBox = bb.max.z - bb.min.z;
  this->setSceneCenter(qglviewer::Vec(0, 0, 0));
  this->setSceneRadius(_maxBBox);
  // updateGL();
  update();
  // this->camera()->showEntireScene();
  // this->ShowStatusMessage("Mesh successfully scale by 0.1", 3000);
}

void QGLTetraViewer::generateOctree(int d) {
  std::cout << "Generating Octree from surface..." << std::endl;
  tMesh->generateOctree((unsigned int)d);
}

void QGLTetraViewer::loadSurface() {
  QString s;
  QFileDialog *qfd =
      new QFileDialog(this, "Select mesh File", "",
                      tr("OBJ (*.obj);; STL (*.stl);; DAE/Collada (*.dae);; "
                         "3DS (*.3ds);; PLY (*.ply);; AC3D (*.ac)"));
  s = qfd->getOpenFileName(
      this, "Select mesh File", "",
      tr("Supported Files (OBJ, STL, Collada, 3DS, PLY, AC) (*.obj *.stl *.dae "
         "*.3ds *.ply *.ac)"));
  delete qfd;

  std::cout << "Loading Surface Mesh... " << s.toStdString() << std::endl;
  if (s.toStdString().empty()) return;
  ShowStatusMessage("Loading Surface...");
  tMesh->LoadSurface(s.toStdString());
  if (!tMesh) {
    std::cerr << "ERROR! Surface Mesh is empty..." << std::endl;
    return;
  }
  BoundingBox bb = tMesh->GetBoundingBox();
  Vec3f bboxCenter = bb.min + ((bb.max - bb.min) / 2.0f);
  tMesh->translateSurfaceMesh(bboxCenter * -1.0);
  bb = tMesh->GetBoundingBox();
  _maxBBox = bb.max.x - bb.min.x;
  if (_maxBBox < (bb.max.y - bb.min.y)) _maxBBox = bb.max.y - bb.min.y;
  if (_maxBBox < (bb.max.z - bb.min.z)) _maxBBox = bb.max.z - bb.min.z;
  this->setSceneCenter(qglviewer::Vec(0, 0, 0));
  this->setSceneRadius(_maxBBox);
  this->camera()->showEntireScene();
  this->ShowStatusMessage("Surface mesh successfully loaded...", 10000);
  emit onLoad();
}

void QGLTetraViewer::loadGMSH() {
  QString s;
  QFileDialog *qfd = new QFileDialog(this, "Select GMSH File", "", "*.msh");
  s = qfd->getOpenFileName(this, "Select GMSH File", "", "*.msh");
  delete qfd;
  std::cout << "Loading GMSH Mesh... " << s.toStdString() << std::endl;
  if (s.toStdString().empty()) return;
  ShowStatusMessage("Loading GMSH...");
  tMesh->LoadGMSH(s.toStdString());
  BoundingBox bb = tMesh->GetBoundingBox();
  _maxBBox = bb.max.x - bb.min.x;
  if (_maxBBox < (bb.max.y - bb.min.y)) _maxBBox = bb.max.y - bb.min.y;
  if (_maxBBox < (bb.max.z - bb.min.z)) _maxBBox = bb.max.z - bb.min.z;
  this->setSceneCenter(qglviewer::Vec(0, 0, 0));
  this->setSceneRadius(_maxBBox);
  this->showEntireScene();
  this->ShowStatusMessage("GMSH successfully loaded...", 10000);
  emit onLoad();
}

void QGLTetraViewer::saveGMSH() {
  if (tMesh == NULL) {
    ShowStatusMessage(
        "Saving GMSH not possible! No tetrahedral mesh present...", 10000);
    return;
  }
  if (tMesh->GetTetraMesh() != NULL) {
    QFileDialog *qfd =
        new QFileDialog(this, "Select GMSH Output File", "", "*.msh");
    QString s =
        qfd->getSaveFileName(this, "Select GMSH Output File", "", "*.msh");
    if (s.toStdString().empty()) {
      delete qfd;
      return;
    }
    qfd->setDefaultSuffix("msh");
    QFileInfo f(s);
    if (f.suffix().isEmpty()) {
      s += ".msh";
    }
    delete qfd;
    std::cout << "Saving GMSH to file: " << s.toStdString() << std::endl;
    tMesh->SaveGMSH(s.toStdString());
  } else {
    ShowStatusMessage("Saving GMSH failed! No tetrahedral mesh present...",
                      10000);
  }
}

void QGLTetraViewer::saveTetgen() {
  if (tMesh == NULL) {
    ShowStatusMessage(
        "Saving Tetgen not possible! No tetrahedral mesh present...", 10000);
    return;
  }
  if (tMesh->GetTetraMesh() != NULL) {
    QFileDialog *qfd =
        new QFileDialog(this, "Select Tetgen Output File prefix", "", "");
    QString s =
        qfd->getSaveFileName(this, "Select Tetgen Output File prefix", "", "");
    if (s.toStdString().empty()) {
      delete qfd;
      return;
    }
    // auto remove file extension if present
    QFileInfo f(s);
    if (!f.suffix().isEmpty()) {
      s = f.baseName();
    }
    delete qfd;
    std::string outfileNames = s.toStdString() + ".node/.ele";
    std::cout << "Saving Tetgen to file: " << outfileNames << std::endl;
    tMesh->SaveTetgen(s.toStdString());
  } else {
    ShowStatusMessage("Saving Tetgen failed! No tetrahedral mesh present...",
                      10000);
  }
}

void QGLTetraViewer::saveSurface() {
  if (tMesh == NULL) {
    ShowStatusMessage("Saving Surface not possible! No mesh present...", 10000);
    return;
  }
  if (tMesh->GetSurface() != NULL) {
    QString selfilter =
        tr("OBJ (*.obj);; STL (*.stl);; Collada(*.dae);; "
           "3DS (*.3ds);; PLY (*.ply)");
    QFileDialog *qfd =
        new QFileDialog(this, "Select Surface Output File", "", selfilter);
    qfd->setDefaultSuffix("obj");

    QString s =
        qfd->getSaveFileName(this, "Select Surface Output File", "", selfilter);
    if (s.toStdString().empty()) {
      delete qfd;
      return;
    }
    std::cout << "Saving Surface to file: " << s.toStdString() << std::endl;
    std::cout << "Selected Type Filter: " << QFileInfo(s).suffix().toStdString()
              << std::endl;
    if (!tMesh->SaveSurface(s.toStdString(),
                            QFileInfo(s).suffix().toStdString()))
      ShowStatusMessage("Saving Surface failed...", 10000);
    delete qfd;
  } else {
    ShowStatusMessage("Saving Surface failed! No surface mesh present...",
                      10000);
  }
}

void QGLTetraViewer::selectSurfaceColor() {
  QColor c =
      QColorDialog::getColor(tMesh->GetSurfaceColor(), this, "Surface Color",
                             QColorDialog::DontUseNativeDialog);
  if (c.isValid()) {
    tMesh->SetSurfaceSolidColor(c.red(), c.green(), c.blue());
    update();
  }
}

void QGLTetraViewer::selectSurfaceColorWireframe() {
  QColor c = QColorDialog::getColor(tMesh->GetSurfaceWireframeColor(), this,
                                    "Surface Wireframe Color",
                                    QColorDialog::DontUseNativeDialog);
  if (c.isValid()) {
    tMesh->SetSurfaceWireframeColor(c.red(), c.green(), c.blue());
    update();
  }
}

void QGLTetraViewer::selectTetraColor() {
  QColor c =
      QColorDialog::getColor(tMesh->GetTetraColor(), this, "Tetra Solid Color",
                             QColorDialog::DontUseNativeDialog);
  if (c.isValid()) {
    tMesh->SetTetraSolidColor(c.red(), c.green(), c.blue());
    update();
  }
}

void QGLTetraViewer::selectTetraColorWireframe() {
  QColor c = QColorDialog::getColor(tMesh->GetTetraWireframeColor(), this,
                                    "Tetra Wireframe Color",
                                    QColorDialog::DontUseNativeDialog);
  if (c.isValid()) {
    tMesh->SetTetraWireframeColor(c.red(), c.green(), c.blue());
    update();
  }
}
