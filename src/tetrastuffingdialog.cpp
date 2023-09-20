/*
 * tetrastuffingdialog.cpp
 *
 *  Created 2012
 *      Author: Dennis Luebke
 */
 
#include "tetrastuffingdialog.h"
#include "ui_tetrastuffingdialog.h"
#include "SofaTetraStuffing.h"
#include <QElapsedTimer>
#include <sstream>

TetraStuffingDialog::TetraStuffingDialog(QWidget *parent) :
    BaseTetraDialog(parent),
    m_ui(new Ui::TetraStuffingDialog)
{
    m_ui->setupUi(this);
    _viewer = NULL;
}

TetraStuffingDialog::~TetraStuffingDialog()
{
    delete m_ui;
}

void TetraStuffingDialog::setTetraSize(const float tetraSize_)
{
    m_ui->tetraSizeSpinBox->setValue(tetraSize_);
}

void TetraStuffingDialog::generateTetras()
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
    SofaTetraStuffing* sts = new SofaTetraStuffing();
    emit displayMessage(QString("Generating tetra mesh..."), 1000);
    sts->GenerateFromSurface(tris, verts, m_ui->tetraSizeSpinBox->value(), m_ui->alphaShortSpinBox->value(), m_ui->alphaLongSpinBox->value(), m_ui->snapToPointsCheckBox->isChecked(), m_ui->splitTetrasCheckBox->isChecked());
    emit displayMessage(QString("Updating visual mesh..."), 2000);
    tMesh_->UpdateTetraMesh(sts->GetTetraVertices(), sts->GetTetras());
    std::stringstream ss;
    ss<<"Generated tetrahedral mesh in "<<t.elapsed()<<"ms. Tetras: "<<sts->GetTetras().size()<<"; Vertices: "<<sts->GetTetraVertices().size();
    std::string stdMSG = ss.str();
    emit notifyDone(QString::fromStdString(stdMSG), 10000);
    delete sts;
}
