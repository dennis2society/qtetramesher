/*
 * cgaltetrahedralizedialog.h
 *
 *  Created 2012
 *      Author: Dennis Luebke
 */

#include "quartetterahedralizedialog.h"
#include "ui_quartettetrastuffingdialog.h"
#include <QElapsedTimer>
#include <sstream>
#include "QuartetTetraStuffing.h"
#include <QMessageBox>

QuartetTetraStuffingDialog::QuartetTetraStuffingDialog(QWidget *parent) :
    BaseTetraDialog(parent),
    m_ui(new Ui::QuartetTetraStuffingDialog)
{
    m_ui->setupUi(this);
    _viewer = NULL;
}


QuartetTetraStuffingDialog::~QuartetTetraStuffingDialog()
{
    delete m_ui;
}

void QuartetTetraStuffingDialog::setParams(const float tetraSize_, const float faceSize_)
{
    m_ui->gridSpacingSpinBox->setValue(tetraSize_);
    m_ui->angleThresholdSpinBox->setValue(faceSize_);
}

void QuartetTetraStuffingDialog::generateTetras()
{

    QElapsedTimer t;
    t.start();
    QGLTetraMesh* tMesh_ = _viewer->tMesh;
    if (tMesh_ == NULL)
    {
        emit displayMessage(QString("ERROR! No surface mesh loaded..."), 5000);
        return;
    }
    const std::vector<Triangle>& tris = tMesh_->GetTriangleTopology()->GetTriangles();
    const std::vector<Vec3f>& verts = tMesh_->GetTriangleTopology()->GetVertices();
    if (tMesh_->GetSurface() == NULL)
    {
        emit displayMessage(QString("ERROR! No surface mesh loaded..."), 5000);
        return;
    }
    std::cout<<"After Crawfaord messages..."<<std::endl;

    QuartetTetraStuffing cdt;
    std::vector<tmpVec3f> tmpVerts;
    std::vector<inTriangle> tmpTris;
    for (unsigned int i=0; i<verts.size(); ++i)
    {
        const Vec3f& org = verts.at(i);
        tmpVec3f tv;
        tv.x = org.x;
        tv.y = org.y;
        tv.z = org.z;
        tmpVerts.push_back(tv);
    }
    for (unsigned int i=0; i<tris.size(); ++i)
    {
        const Triangle& org = tris.at(i);
        inTriangle inT;
        inT.index[0] = org.index[0];
        inT.index[1] = org.index[1];
        inT.index[2] = org.index[2];
        tmpTris.push_back(inT);
    }
    emit displayMessage(QString("Generating tetra mesh..."), 1000);
    cdt.GenerateFromSurface(tmpTris, tmpVerts, m_ui->gridSpacingSpinBox->value(), m_ui->angleThresholdSpinBox->value(), true);
    const std::vector<outTetra>& outTets = cdt.GetTetras();
    const std::vector<tmpVec3f>& outVerts = cdt.GetTetraVertices();
    std::vector<Vec3f> finalVerts;
    std::vector<Tetrahedron> finalTets;
    for (unsigned int i=0; i<outVerts.size(); ++i)
    {
        const tmpVec3f& org = outVerts.at(i);
        Vec3f v;
        v.x = org.x;
        v.y = org.y;
        v.z = org.z;
        finalVerts.push_back(v);
    }
    for (unsigned int i=0; i<outTets.size(); ++i)
    {
        const outTetra& org = outTets.at(i);
        Tetrahedron t;
        t.index[0] = org.index[0];
        t.index[1] = org.index[1];
        t.index[2] = org.index[2];
        t.index[3] = org.index[3];
        finalTets.push_back(t);
    }
    tMesh_->UpdateTetraMesh(finalVerts, finalTets);
    std::stringstream ss;
    ss<<"Generated tetrahedral mesh in "<<t.elapsed()<<"ms. Tetras: "<<cdt.GetTetras().size()<<"; Vertices: "<<cdt.GetTetraVertices().size();
    std::string stdMSG = ss.str();
    emit notifyDone(QString::fromStdString(stdMSG), 10000);

}
