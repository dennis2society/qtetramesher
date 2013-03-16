/*
 * qgltetraviewer.cpp
 *
 * Derived from QGLViewer to allow custom mesh drawing.
 *
 *  Created 2012
 *      Author: Dennis Luebke
 */

#include "qgltetraviewer.h"
#include <math.h>
#include <sstream>
#include <string>
#include <QFileDialog>
#include <QColorDialog>

// Constructor must call the base class constructor.
#if QT_VERSION < 0x040000
QGLTetraViewer::QGLTetraViewer(QWidget *parent, const char *name)
  : QGLViewer(parent, name)
#else
QGLTetraViewer::QGLTetraViewer(QWidget *parent)
    : QGLViewer(parent)
#endif
{
	//restoreStateFromFile();
	tMesh = new QGLTetraMesh();
}

const float QGLTetraViewer::getMaxBBox()
{
    return _maxBBox;
}

void QGLTetraViewer::draw()
{  
	if (tMesh->IsReady())
	{
        BoundingBox bb = tMesh->GetBoundingBox();
		tMesh->Draw();
		std::stringstream ss;
		std::string stdMSG;
		QString msg;
		if (tMesh->GetSurface() != NULL)
		{
			ss<<"Surface Mesh: "<<"Vertices: "<<tMesh->GetSurface()->GetVertices().size()<<"; Triangles: "<<tMesh->GetSurface()->GetTriangles().size();
			stdMSG = ss.str();
			msg = QString::fromStdString(stdMSG);
			glColor3f(0.1f, 0.8f, 0.1f);
			drawText(10, 30, msg);
			ss.str("");
            _maxBBox = bb.max.x - bb.min.x;
            if (_maxBBox < (bb.max.y - bb.min.y))
                _maxBBox = bb.max.y - bb.min.y;
            if (_maxBBox < (bb.max.z - bb.min.z))
                _maxBBox = bb.max.z - bb.min.z;
            ss<<"BBox Min:"<<bb.min.x<<"/"<<bb.min.y<<"/"<<bb.min.z<<"; BBox Max: "<<bb.max.x<<"/"<<bb.max.y<<"/"<<bb.max.z<<"; Max Value: "<<_maxBBox;
			stdMSG = ss.str();
			msg = QString::fromStdString(stdMSG);
			glColor3f(0.1f, 0.8f, 0.1f);
			drawText(10, 45, msg);
			ss.str("");
		}
		if (tMesh->GetTetraMesh() != NULL)
		{
			ss<<"Tetra Mesh: "<<"Vertices: "<<tMesh->GetTetraMesh()->GetVertices().size()<<"; Tetras: "<<tMesh->GetTetraMesh()->GetTetrahedra().size();
			stdMSG = ss.str();
			msg = QString::fromStdString(stdMSG);
			glColor3f(0.8f, 0.8f, 0.1f);
			drawText(10, 60, msg);
			ss.str("");
			ss<<"Tetra Mesh: "<<"Triangles: "<<tMesh->GetTetraMesh()->GetTriangles().size()<<"; Edges: "<<tMesh->GetTetraMesh()->GetEdges().size();
			stdMSG = ss.str();
			msg = QString::fromStdString(stdMSG);
			glColor3f(0.8f, 0.8f, 0.1f);
			drawText(10, 75, msg);
		}
	}
	else
	{
		//// Draws a spiral
		const float nbSteps = 200.0;
		glBegin(GL_QUAD_STRIP);
		for (float i=0; i<nbSteps; ++i)
	    {
		    float ratio = i/nbSteps;
		    float angle = 21.0*ratio;
		    float c = cos(angle);
		    float s = sin(angle);
		    float r1 = 1.0 - 0.8*ratio;
		    float r2 = 0.8 - 0.8*ratio;
		    float alt = ratio - 0.5;
		    const float nor = .5;
		    const float up = sqrt(1.0-nor*nor);
		    glColor3f(1.0-ratio, 0.2f , ratio);
		    glNormal3f(nor*c, up, nor*s);
		    glVertex3f(r1*c, alt, r1*s);
		    glVertex3f(r2*c, alt+0.05, r2*s);
		}
		glEnd();	
	}
}

QString QGLTetraViewer::helpString() const
{
  QString text("<h2>dennis2society.de QGL Tetra Mesh Viewer</h2>");
  text += "<br>";
  text += "<br>";
  text += "This program can load and show surface and tetrahedral meshes in Wavefront OBJ, GMSH and Nvidia's NXUStream2 format.<br>";
  text += "<br>";
  return text;
}

void QGLTetraViewer::ToggleTetraVis(int i)
{
	tMesh->ToggleTetraMesh(i);
	updateGL();
}

void QGLTetraViewer::ToggleTriangleVis(int i)
{
	tMesh->ToggleTriangleMesh(i);
	updateGL();
}

void QGLTetraViewer::ToggleOctreeVis(int i)
{
    std::cout<<"Octree Vis toggled..."<<std::endl;
    tMesh->ToggelOctreeVis(i);
    updateGL();
}

void QGLTetraViewer::ToggleBBox(int i)
{
	tMesh->ToggleBBox(i);
	updateGL();
}

void QGLTetraViewer::SetCutPlane(int i)
{
	tMesh->SetCutPlaneOffset(i);
	updateGL();
}


void QGLTetraViewer::ShowStatusMessage(const QString& msg_, int duration_)
{
	displayMessage(msg_, duration_);
}

void QGLTetraViewer::scale10()
{
    if (tMesh == NULL)
    {
        std::cerr<<"Error! No mesh available..."<<std::endl;
        return;
    }
    tMesh->scalex10();
    BoundingBox bb = tMesh->GetBoundingBox();
    _maxBBox = bb.max.x - bb.min.x;
    if (_maxBBox < (bb.max.y - bb.min.y))
        _maxBBox = bb.max.y - bb.min.y;
    if (_maxBBox < (bb.max.z - bb.min.z))
        _maxBBox = bb.max.z - bb.min.z;
    this->setSceneCenter(qglviewer::Vec(0,0,0));
    this->setSceneRadius(_maxBBox);
    updateGL();
    //this->camera()->showEntireScene();
    //this->ShowStatusMessage("Mesh successfully scale by 10", 3000);
}

void QGLTetraViewer::scale01()
{
    if (tMesh == NULL)
    {
        std::cerr<<"Error! No mesh available..."<<std::endl;
        return;
    }
    tMesh->scalex01();
    BoundingBox bb = tMesh->GetBoundingBox();
    _maxBBox = bb.max.x - bb.min.x;
    if (_maxBBox < (bb.max.y - bb.min.y))
        _maxBBox = bb.max.y - bb.min.y;
    if (_maxBBox < (bb.max.z - bb.min.z))
        _maxBBox = bb.max.z - bb.min.z;
    this->setSceneCenter(qglviewer::Vec(0,0,0));
    this->setSceneRadius(_maxBBox);
    updateGL();
    //this->camera()->showEntireScene();
    //this->ShowStatusMessage("Mesh successfully scale by 0.1", 3000);
}

void QGLTetraViewer::generateOctree(int d)
{
    std::cout<<"Generating Octree from surface..."<<std::endl;
    tMesh->generateOctree((unsigned int)d);
}

void QGLTetraViewer::loadSurface()
{
    QFileDialog* qfd = new QFileDialog(this, "Select mesh File", "", tr("Surface Meshes (*.obj *.off *.ply *.stl)"));
    QString s = qfd->getOpenFileName(this, "Select mesh File", "", tr("Surface Meshes (*.obj *.off *.ply *.stl)"));
    std::cout<<"Loading Surface Mesh... "<<s.toStdString()<<std::endl;
    delete qfd;
    if (s.toStdString().empty())
        return;
    ShowStatusMessage("Loading Surface...");
    tMesh->LoadSurface(s.toStdString());
    BoundingBox bb = tMesh->GetBoundingBox();
    _maxBBox = bb.max.x - bb.min.x;
    if (_maxBBox < (bb.max.y - bb.min.y))
        _maxBBox = bb.max.y - bb.min.y;
    if (_maxBBox < (bb.max.z - bb.min.z))
        _maxBBox = bb.max.z - bb.min.z;
    this->setSceneCenter(qglviewer::Vec(0,0,0));
    this->setSceneRadius(_maxBBox);
    this->camera()->showEntireScene();
    this->ShowStatusMessage("OBJ successfully loaded...", 3000);
    emit onLoad();
}

void QGLTetraViewer::loadGMSH()
{
    QFileDialog* qfd = new QFileDialog(this, "Select GMSH File", "", "*.msh");
    QString s = qfd->getOpenFileName(this, "Select GMSH File", "", "*.msh");
    std::cout<<"Loading GMSH Mesh... "<<s.toStdString()<<std::endl;
    delete qfd;
    if (s.toStdString().empty())
        return;
    ShowStatusMessage("Loading GMSH...");
    tMesh->ClearSurface();
    tMesh->LoadGMSH(s.toStdString());
    BoundingBox bb = tMesh->GetBoundingBox();
    float _maxBBox = bb.max.x - bb.min.x;
    if (_maxBBox < (bb.max.y - bb.min.y))
        _maxBBox = bb.max.y - bb.min.y;
    if (_maxBBox < (bb.max.z - bb.min.z))
        _maxBBox = bb.max.z - bb.min.z;
    this->setSceneCenter(qglviewer::Vec(0,0,0));
    this->setSceneRadius(_maxBBox);
    this->showEntireScene();
    this->ShowStatusMessage("GMSH successfully loaded...", 3000);
    emit onLoad();
}

void QGLTetraViewer::saveGMSH()
{
    if (tMesh == NULL)
    {
        ShowStatusMessage("Saving GMSH not possible! No tetrahedral mesh present...");
        return;
    }
    if (tMesh->GetTetraMesh() != NULL)
    {
        QFileDialog* qfd = new QFileDialog(this, "Select GMSH Output File", "", "*.msh");
        QString s = qfd->getSaveFileName(this, "Select GMSH Output File", "", "*.msh");
        if (s.toStdString().empty())
        {
            delete qfd;
            return;
        }
        qfd->setDefaultSuffix("msh");
        QFileInfo f(s);
        if (f.suffix().isEmpty())
        {
            s += ".msh";
        }
        delete qfd;
        std::cout<<"Saving GMSH to file: "<<s.toStdString()<<std::endl;
        tMesh->SaveGMSH(s.toStdString());
    }
    else
    {
        ShowStatusMessage("Saving GMSH failed! No tetrahedral mesh present...");
    }
}

void QGLTetraViewer::saveSurface()
{
    if (tMesh == NULL)
    {
        ShowStatusMessage("Saving Surface not possible! No mesh present...");
        return;
    }
    if (tMesh->GetSurface() != NULL)
    {
        QFileDialog* qfd = new QFileDialog(this, "Select Surface Output File", "", tr("Surface Meshes (*.obj *.off *.ply *.stl)"));
        QString s = qfd->getSaveFileName(this, "Select Surface Output File", "", tr("Surface Meshes (*.obj *.off *.ply *.stl)"));
        if (s.toStdString().empty())
        {
            delete qfd;
            return;
        }
        std::cout<<"Saving Surface to file: "<<s.toStdString()<<std::endl;
        delete qfd;
        if (!tMesh->SaveSurface(s.toStdString()))
            ShowStatusMessage("Saving Surface failed...");
    }
    else
    {
        ShowStatusMessage("Saving Surface failed! No surface mesh present...");
    }
}

void QGLTetraViewer::selectSurfaceColor()
{
    QColorDialog* qcd = new QColorDialog(this);
    QColor c = qcd->getColor(tMesh->GetSurfaceColor(), this);
    if (c.isValid())
    {
        tMesh->SetSurfaceSolidColor(c.red(), c.green(), c.blue());
    }
    delete qcd;
}

void QGLTetraViewer::selectSurfaceColorWireframe()
{
    QColorDialog* qcd = new QColorDialog(this);
    QColor c = qcd->getColor(tMesh->GetSurfaceWireframeColor(), this);
    if (c.isValid())
    {
        tMesh->SetSurfaceWireframeColor(c.red(), c.green(), c.blue());
    }
    delete qcd;
}

void QGLTetraViewer::selectTetraColor()
{
    QColorDialog* qcd = new QColorDialog(this);
    QColor c = qcd->getColor(tMesh->GetTetraColor(), this);
    if (c.isValid())
    {
        tMesh->SetTetraSolidColor(c.red(), c.green(), c.blue());
    }
    delete qcd;
}

void QGLTetraViewer::selectTetraColorWireframe()
{
    QColorDialog* qcd = new QColorDialog(this);
    QColor c = qcd->getColor(tMesh->GetTetraWireframeColor(), this);
    if (c.isValid())
    {
        tMesh->SetTetraWireframeColor(c.red(), c.green(), c.blue());
    }
    delete qcd;
}
