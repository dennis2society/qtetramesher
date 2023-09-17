/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.10
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "QGLViewer/qglviewer.h"
#include "qgltetraviewer.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionLoad_GMSH;
    QAction *actionLoad_NXU2;
    QAction *actionLoad_Tetgen;
    QAction *actionExport_OBJ;
    QAction *actionExport_GMSH;
    QAction *actionExport_Tetgen;
    QAction *actionQuit;
    QAction *actionExport_Surface;
    QAction *actionExport_GMSH_2;
    QAction *actionExport_Tetgen_2;
    QAction *actionShow_FPS;
    QAction *actionShow_Grid;
    QAction *actionShow_Axis;
    QAction *actionHelp;
    QAction *actionAbout;
    QAction *actionFullscreen;
    QAction *actionAbout_QGLViewer;
    QAction *actionMeshTetraStuffing;
    QAction *actionTetGen;
    QAction *actionCGAL_Tetrahedralize;
    QAction *actionLoad_Surface_OBJ_OFF_3DS_PLY;
    QAction *actionScale_x10;
    QAction *actionScale_x_0_1;
    QAction *actionGenerate_Octree;
    QAction *actionQuartetTetraStuffing;
    QAction *actionUpdate_Z_Slider;
    QWidget *centralWidget;
    QGridLayout *gridLayout;
    QHBoxLayout *mainLayout;
    QFrame *viewAndControlFrame;
    QGridLayout *gridLayout_3;
    QVBoxLayout *viewAndControlLayout;
    QFrame *qglViewerFrame;
    QGridLayout *gridLayout_4;
    QGLTetraViewer *viewer;
    QFrame *visualizationFrame;
    QHBoxLayout *horizontalLayout_3;
    QGridLayout *gridLayout_2;
    QFrame *octreeFrame;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *octreeVisLayout;
    QLabel *octreeVisLabel;
    QComboBox *octreeCombo;
    QLabel *octreeDepthLabel;
    QHBoxLayout *horizontalLayout_5;
    QSpinBox *octreeDepthSpinbox;
    QPushButton *octreeGenerateButton;
    QFrame *boundingBoxFrame;
    QHBoxLayout *horizontalLayout_4;
    QVBoxLayout *verticalLayout_2;
    QLabel *boundingVisLabel;
    QComboBox *boundingBoxCombo;
    QFrame *surfaceVisFrame;
    QHBoxLayout *horizontalLayout_2;
    QVBoxLayout *surfaceVisLayout;
    QLabel *surfaceVisLabel;
    QComboBox *surfaceCombo;
    QPushButton *surfaceColorButton;
    QPushButton *surfaceWireframeColorButton;
    QFrame *tetraVisFrame;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout;
    QLabel *tetraVisLabel;
    QComboBox *tetraCombo;
    QPushButton *tetraSolidColorButton;
    QPushButton *tetraWireframeColorButton;
    QFrame *line;
    QLabel *cutPlaneLabel;
    QSlider *cutPlaneSlider;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuHelp_Info;
    QMenu *menuView;
    QMenu *menuMesh;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1024, 829);
        QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        sizePolicy.setHorizontalStretch(1);
        sizePolicy.setVerticalStretch(1);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        MainWindow->setMinimumSize(QSize(0, 0));
        actionLoad_GMSH = new QAction(MainWindow);
        actionLoad_GMSH->setObjectName(QString::fromUtf8("actionLoad_GMSH"));
        actionLoad_NXU2 = new QAction(MainWindow);
        actionLoad_NXU2->setObjectName(QString::fromUtf8("actionLoad_NXU2"));
        actionLoad_Tetgen = new QAction(MainWindow);
        actionLoad_Tetgen->setObjectName(QString::fromUtf8("actionLoad_Tetgen"));
        actionExport_OBJ = new QAction(MainWindow);
        actionExport_OBJ->setObjectName(QString::fromUtf8("actionExport_OBJ"));
        actionExport_GMSH = new QAction(MainWindow);
        actionExport_GMSH->setObjectName(QString::fromUtf8("actionExport_GMSH"));
        actionExport_Tetgen = new QAction(MainWindow);
        actionExport_Tetgen->setObjectName(QString::fromUtf8("actionExport_Tetgen"));
        actionQuit = new QAction(MainWindow);
        actionQuit->setObjectName(QString::fromUtf8("actionQuit"));
        actionExport_Surface = new QAction(MainWindow);
        actionExport_Surface->setObjectName(QString::fromUtf8("actionExport_Surface"));
        actionExport_GMSH_2 = new QAction(MainWindow);
        actionExport_GMSH_2->setObjectName(QString::fromUtf8("actionExport_GMSH_2"));
        actionExport_Tetgen_2 = new QAction(MainWindow);
        actionExport_Tetgen_2->setObjectName(QString::fromUtf8("actionExport_Tetgen_2"));
        actionShow_FPS = new QAction(MainWindow);
        actionShow_FPS->setObjectName(QString::fromUtf8("actionShow_FPS"));
        actionShow_FPS->setCheckable(true);
        actionShow_FPS->setChecked(true);
        actionShow_Grid = new QAction(MainWindow);
        actionShow_Grid->setObjectName(QString::fromUtf8("actionShow_Grid"));
        actionShow_Grid->setCheckable(true);
        actionShow_Grid->setChecked(true);
        actionShow_Axis = new QAction(MainWindow);
        actionShow_Axis->setObjectName(QString::fromUtf8("actionShow_Axis"));
        actionShow_Axis->setCheckable(true);
        actionShow_Axis->setChecked(true);
        actionHelp = new QAction(MainWindow);
        actionHelp->setObjectName(QString::fromUtf8("actionHelp"));
        actionAbout = new QAction(MainWindow);
        actionAbout->setObjectName(QString::fromUtf8("actionAbout"));
        actionFullscreen = new QAction(MainWindow);
        actionFullscreen->setObjectName(QString::fromUtf8("actionFullscreen"));
        actionFullscreen->setCheckable(true);
        actionAbout_QGLViewer = new QAction(MainWindow);
        actionAbout_QGLViewer->setObjectName(QString::fromUtf8("actionAbout_QGLViewer"));
        actionMeshTetraStuffing = new QAction(MainWindow);
        actionMeshTetraStuffing->setObjectName(QString::fromUtf8("actionMeshTetraStuffing"));
        actionTetGen = new QAction(MainWindow);
        actionTetGen->setObjectName(QString::fromUtf8("actionTetGen"));
        actionCGAL_Tetrahedralize = new QAction(MainWindow);
        actionCGAL_Tetrahedralize->setObjectName(QString::fromUtf8("actionCGAL_Tetrahedralize"));
        actionLoad_Surface_OBJ_OFF_3DS_PLY = new QAction(MainWindow);
        actionLoad_Surface_OBJ_OFF_3DS_PLY->setObjectName(QString::fromUtf8("actionLoad_Surface_OBJ_OFF_3DS_PLY"));
        actionScale_x10 = new QAction(MainWindow);
        actionScale_x10->setObjectName(QString::fromUtf8("actionScale_x10"));
        actionScale_x_0_1 = new QAction(MainWindow);
        actionScale_x_0_1->setObjectName(QString::fromUtf8("actionScale_x_0_1"));
        actionGenerate_Octree = new QAction(MainWindow);
        actionGenerate_Octree->setObjectName(QString::fromUtf8("actionGenerate_Octree"));
        actionQuartetTetraStuffing = new QAction(MainWindow);
        actionQuartetTetraStuffing->setObjectName(QString::fromUtf8("actionQuartetTetraStuffing"));
        actionUpdate_Z_Slider = new QAction(MainWindow);
        actionUpdate_Z_Slider->setObjectName(QString::fromUtf8("actionUpdate_Z_Slider"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        QSizePolicy sizePolicy1(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(centralWidget->sizePolicy().hasHeightForWidth());
        centralWidget->setSizePolicy(sizePolicy1);
        centralWidget->setMinimumSize(QSize(1024, 768));
        centralWidget->setSizeIncrement(QSize(0, 0));
        gridLayout = new QGridLayout(centralWidget);
        gridLayout->setSpacing(1);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(2, 0, 1, 1);
        mainLayout = new QHBoxLayout();
        mainLayout->setSpacing(1);
        mainLayout->setObjectName(QString::fromUtf8("mainLayout"));
        viewAndControlFrame = new QFrame(centralWidget);
        viewAndControlFrame->setObjectName(QString::fromUtf8("viewAndControlFrame"));
        viewAndControlFrame->setFrameShape(QFrame::Box);
        viewAndControlFrame->setFrameShadow(QFrame::Raised);
        gridLayout_3 = new QGridLayout(viewAndControlFrame);
        gridLayout_3->setSpacing(1);
        gridLayout_3->setContentsMargins(11, 11, 11, 11);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        gridLayout_3->setContentsMargins(1, 1, 1, 1);
        viewAndControlLayout = new QVBoxLayout();
        viewAndControlLayout->setSpacing(1);
        viewAndControlLayout->setObjectName(QString::fromUtf8("viewAndControlLayout"));
        qglViewerFrame = new QFrame(viewAndControlFrame);
        qglViewerFrame->setObjectName(QString::fromUtf8("qglViewerFrame"));
        qglViewerFrame->setFrameShape(QFrame::Box);
        qglViewerFrame->setFrameShadow(QFrame::Raised);
        gridLayout_4 = new QGridLayout(qglViewerFrame);
        gridLayout_4->setSpacing(1);
        gridLayout_4->setContentsMargins(11, 11, 11, 11);
        gridLayout_4->setObjectName(QString::fromUtf8("gridLayout_4"));
        gridLayout_4->setContentsMargins(1, 1, 1, 1);
        viewer = new QGLTetraViewer(qglViewerFrame);
        viewer->setObjectName(QString::fromUtf8("viewer"));

        gridLayout_4->addWidget(viewer, 0, 0, 1, 1);


        viewAndControlLayout->addWidget(qglViewerFrame);


        gridLayout_3->addLayout(viewAndControlLayout, 0, 0, 1, 1);


        mainLayout->addWidget(viewAndControlFrame);

        visualizationFrame = new QFrame(centralWidget);
        visualizationFrame->setObjectName(QString::fromUtf8("visualizationFrame"));
        QSizePolicy sizePolicy2(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(visualizationFrame->sizePolicy().hasHeightForWidth());
        visualizationFrame->setSizePolicy(sizePolicy2);
        visualizationFrame->setMinimumSize(QSize(200, 0));
        visualizationFrame->setLayoutDirection(Qt::LeftToRight);
        visualizationFrame->setFrameShape(QFrame::Box);
        visualizationFrame->setFrameShadow(QFrame::Sunken);
        visualizationFrame->setLineWidth(2);
        horizontalLayout_3 = new QHBoxLayout(visualizationFrame);
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(1, 1, 1, 1);
        gridLayout_2 = new QGridLayout();
        gridLayout_2->setSpacing(1);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout_2->setContentsMargins(1, 1, 1, 1);
        octreeFrame = new QFrame(visualizationFrame);
        octreeFrame->setObjectName(QString::fromUtf8("octreeFrame"));
        QSizePolicy sizePolicy3(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(octreeFrame->sizePolicy().hasHeightForWidth());
        octreeFrame->setSizePolicy(sizePolicy3);
        octreeFrame->setMinimumSize(QSize(0, 120));
        octreeFrame->setFrameShape(QFrame::Box);
        octreeFrame->setFrameShadow(QFrame::Sunken);
        verticalLayoutWidget = new QWidget(octreeFrame);
        verticalLayoutWidget->setObjectName(QString::fromUtf8("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(0, 0, 208, 118));
        octreeVisLayout = new QVBoxLayout(verticalLayoutWidget);
        octreeVisLayout->setSpacing(1);
        octreeVisLayout->setContentsMargins(11, 11, 11, 11);
        octreeVisLayout->setObjectName(QString::fromUtf8("octreeVisLayout"));
        octreeVisLayout->setContentsMargins(1, 1, 1, 1);
        octreeVisLabel = new QLabel(verticalLayoutWidget);
        octreeVisLabel->setObjectName(QString::fromUtf8("octreeVisLabel"));
        sizePolicy3.setHeightForWidth(octreeVisLabel->sizePolicy().hasHeightForWidth());
        octreeVisLabel->setSizePolicy(sizePolicy3);
        octreeVisLabel->setMinimumSize(QSize(0, 30));
        QFont font;
        font.setPointSize(12);
        font.setBold(true);
        octreeVisLabel->setFont(font);
        octreeVisLabel->setAlignment(Qt::AlignCenter);
        octreeVisLabel->setMargin(2);

        octreeVisLayout->addWidget(octreeVisLabel);

        octreeCombo = new QComboBox(verticalLayoutWidget);
        octreeCombo->addItem(QString());
        octreeCombo->addItem(QString());
        octreeCombo->addItem(QString());
        octreeCombo->setObjectName(QString::fromUtf8("octreeCombo"));
        sizePolicy3.setHeightForWidth(octreeCombo->sizePolicy().hasHeightForWidth());
        octreeCombo->setSizePolicy(sizePolicy3);
        octreeCombo->setMinimumSize(QSize(0, 30));
        QFont font1;
        font1.setPointSize(10);
        font1.setBold(true);
        octreeCombo->setFont(font1);
        octreeCombo->setFrame(true);

        octreeVisLayout->addWidget(octreeCombo);

        octreeDepthLabel = new QLabel(verticalLayoutWidget);
        octreeDepthLabel->setObjectName(QString::fromUtf8("octreeDepthLabel"));
        QFont font2;
        font2.setBold(true);
        octreeDepthLabel->setFont(font2);

        octreeVisLayout->addWidget(octreeDepthLabel);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        octreeDepthSpinbox = new QSpinBox(verticalLayoutWidget);
        octreeDepthSpinbox->setObjectName(QString::fromUtf8("octreeDepthSpinbox"));
        QSizePolicy sizePolicy4(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(octreeDepthSpinbox->sizePolicy().hasHeightForWidth());
        octreeDepthSpinbox->setSizePolicy(sizePolicy4);
        octreeDepthSpinbox->setMinimumSize(QSize(80, 0));
        octreeDepthSpinbox->setFont(font2);
        octreeDepthSpinbox->setLayoutDirection(Qt::LeftToRight);
        octreeDepthSpinbox->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        octreeDepthSpinbox->setMinimum(1);
        octreeDepthSpinbox->setMaximum(16);
        octreeDepthSpinbox->setValue(6);

        horizontalLayout_5->addWidget(octreeDepthSpinbox);

        octreeGenerateButton = new QPushButton(verticalLayoutWidget);
        octreeGenerateButton->setObjectName(QString::fromUtf8("octreeGenerateButton"));
        octreeGenerateButton->setFont(font2);

        horizontalLayout_5->addWidget(octreeGenerateButton);


        octreeVisLayout->addLayout(horizontalLayout_5);


        gridLayout_2->addWidget(octreeFrame, 5, 0, 1, 1);

        boundingBoxFrame = new QFrame(visualizationFrame);
        boundingBoxFrame->setObjectName(QString::fromUtf8("boundingBoxFrame"));
        sizePolicy3.setHeightForWidth(boundingBoxFrame->sizePolicy().hasHeightForWidth());
        boundingBoxFrame->setSizePolicy(sizePolicy3);
        boundingBoxFrame->setMinimumSize(QSize(0, 100));
        boundingBoxFrame->setFrameShape(QFrame::Box);
        boundingBoxFrame->setFrameShadow(QFrame::Sunken);
        horizontalLayout_4 = new QHBoxLayout(boundingBoxFrame);
        horizontalLayout_4->setSpacing(1);
        horizontalLayout_4->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(1, 1, 1, -1);
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(1);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(1, 1, 1, 1);
        boundingVisLabel = new QLabel(boundingBoxFrame);
        boundingVisLabel->setObjectName(QString::fromUtf8("boundingVisLabel"));
        boundingVisLabel->setFont(font);
        boundingVisLabel->setAlignment(Qt::AlignCenter);

        verticalLayout_2->addWidget(boundingVisLabel);

        boundingBoxCombo = new QComboBox(boundingBoxFrame);
        boundingBoxCombo->addItem(QString());
        boundingBoxCombo->addItem(QString());
        boundingBoxCombo->addItem(QString());
        boundingBoxCombo->setObjectName(QString::fromUtf8("boundingBoxCombo"));
        boundingBoxCombo->setMinimumSize(QSize(0, 30));
        boundingBoxCombo->setFont(font1);

        verticalLayout_2->addWidget(boundingBoxCombo);


        horizontalLayout_4->addLayout(verticalLayout_2);


        gridLayout_2->addWidget(boundingBoxFrame, 4, 0, 1, 1);

        surfaceVisFrame = new QFrame(visualizationFrame);
        surfaceVisFrame->setObjectName(QString::fromUtf8("surfaceVisFrame"));
        sizePolicy3.setHeightForWidth(surfaceVisFrame->sizePolicy().hasHeightForWidth());
        surfaceVisFrame->setSizePolicy(sizePolicy3);
        surfaceVisFrame->setMinimumSize(QSize(0, 100));
        surfaceVisFrame->setFrameShape(QFrame::Box);
        surfaceVisFrame->setFrameShadow(QFrame::Sunken);
        horizontalLayout_2 = new QHBoxLayout(surfaceVisFrame);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(2, 2, 2, 2);
        surfaceVisLayout = new QVBoxLayout();
        surfaceVisLayout->setSpacing(1);
        surfaceVisLayout->setObjectName(QString::fromUtf8("surfaceVisLayout"));
        surfaceVisLayout->setContentsMargins(1, 1, 1, 1);
        surfaceVisLabel = new QLabel(surfaceVisFrame);
        surfaceVisLabel->setObjectName(QString::fromUtf8("surfaceVisLabel"));
        surfaceVisLabel->setFont(font);
        surfaceVisLabel->setFrameShape(QFrame::NoFrame);
        surfaceVisLabel->setAlignment(Qt::AlignCenter);
        surfaceVisLabel->setMargin(2);

        surfaceVisLayout->addWidget(surfaceVisLabel);

        surfaceCombo = new QComboBox(surfaceVisFrame);
        surfaceCombo->addItem(QString());
        surfaceCombo->addItem(QString());
        surfaceCombo->addItem(QString());
        surfaceCombo->setObjectName(QString::fromUtf8("surfaceCombo"));
        surfaceCombo->setMinimumSize(QSize(0, 30));
        surfaceCombo->setFont(font1);

        surfaceVisLayout->addWidget(surfaceCombo);

        surfaceColorButton = new QPushButton(surfaceVisFrame);
        surfaceColorButton->setObjectName(QString::fromUtf8("surfaceColorButton"));

        surfaceVisLayout->addWidget(surfaceColorButton);

        surfaceWireframeColorButton = new QPushButton(surfaceVisFrame);
        surfaceWireframeColorButton->setObjectName(QString::fromUtf8("surfaceWireframeColorButton"));

        surfaceVisLayout->addWidget(surfaceWireframeColorButton);


        horizontalLayout_2->addLayout(surfaceVisLayout);


        gridLayout_2->addWidget(surfaceVisFrame, 1, 0, 1, 1);

        tetraVisFrame = new QFrame(visualizationFrame);
        tetraVisFrame->setObjectName(QString::fromUtf8("tetraVisFrame"));
        sizePolicy3.setHeightForWidth(tetraVisFrame->sizePolicy().hasHeightForWidth());
        tetraVisFrame->setSizePolicy(sizePolicy3);
        tetraVisFrame->setMinimumSize(QSize(0, 200));
        tetraVisFrame->setFrameShape(QFrame::Box);
        tetraVisFrame->setFrameShadow(QFrame::Sunken);
        horizontalLayout = new QHBoxLayout(tetraVisFrame);
        horizontalLayout->setSpacing(1);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(1, 1, 1, 1);
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(1);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(1, 1, 1, 1);
        tetraVisLabel = new QLabel(tetraVisFrame);
        tetraVisLabel->setObjectName(QString::fromUtf8("tetraVisLabel"));
        tetraVisLabel->setFont(font);
        tetraVisLabel->setTextFormat(Qt::AutoText);
        tetraVisLabel->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(tetraVisLabel);

        tetraCombo = new QComboBox(tetraVisFrame);
        tetraCombo->addItem(QString());
        tetraCombo->addItem(QString());
        tetraCombo->addItem(QString());
        tetraCombo->setObjectName(QString::fromUtf8("tetraCombo"));
        tetraCombo->setMinimumSize(QSize(0, 30));
        tetraCombo->setFont(font1);

        verticalLayout->addWidget(tetraCombo);

        tetraSolidColorButton = new QPushButton(tetraVisFrame);
        tetraSolidColorButton->setObjectName(QString::fromUtf8("tetraSolidColorButton"));

        verticalLayout->addWidget(tetraSolidColorButton);

        tetraWireframeColorButton = new QPushButton(tetraVisFrame);
        tetraWireframeColorButton->setObjectName(QString::fromUtf8("tetraWireframeColorButton"));

        verticalLayout->addWidget(tetraWireframeColorButton);

        line = new QFrame(tetraVisFrame);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line);

        cutPlaneLabel = new QLabel(tetraVisFrame);
        cutPlaneLabel->setObjectName(QString::fromUtf8("cutPlaneLabel"));
        QFont font3;
        font3.setPointSize(10);
        font3.setBold(false);
        cutPlaneLabel->setFont(font3);
        cutPlaneLabel->setFrameShape(QFrame::NoFrame);
        cutPlaneLabel->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        verticalLayout->addWidget(cutPlaneLabel);

        cutPlaneSlider = new QSlider(tetraVisFrame);
        cutPlaneSlider->setObjectName(QString::fromUtf8("cutPlaneSlider"));
        cutPlaneSlider->setOrientation(Qt::Horizontal);

        verticalLayout->addWidget(cutPlaneSlider);


        horizontalLayout->addLayout(verticalLayout);


        gridLayout_2->addWidget(tetraVisFrame, 2, 0, 1, 1);


        horizontalLayout_3->addLayout(gridLayout_2);


        mainLayout->addWidget(visualizationFrame);


        gridLayout->addLayout(mainLayout, 0, 1, 1, 1);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1024, 26));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        menuHelp_Info = new QMenu(menuBar);
        menuHelp_Info->setObjectName(QString::fromUtf8("menuHelp_Info"));
        menuView = new QMenu(menuBar);
        menuView->setObjectName(QString::fromUtf8("menuView"));
        menuMesh = new QMenu(menuBar);
        menuMesh->setObjectName(QString::fromUtf8("menuMesh"));
        MainWindow->setMenuBar(menuBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuMesh->menuAction());
        menuBar->addAction(menuView->menuAction());
        menuBar->addAction(menuHelp_Info->menuAction());
        menuFile->addAction(actionLoad_Surface_OBJ_OFF_3DS_PLY);
        menuFile->addAction(actionLoad_GMSH);
        menuFile->addAction(actionLoad_NXU2);
        menuFile->addAction(actionLoad_Tetgen);
        menuFile->addSeparator();
        menuFile->addAction(actionExport_Surface);
        menuFile->addAction(actionExport_GMSH_2);
        menuFile->addAction(actionExport_Tetgen_2);
        menuFile->addSeparator();
        menuFile->addAction(actionQuit);
        menuHelp_Info->addAction(actionHelp);
        menuHelp_Info->addAction(actionAbout_QGLViewer);
        menuView->addAction(actionShow_FPS);
        menuView->addAction(actionShow_Grid);
        menuView->addAction(actionShow_Axis);
        menuView->addAction(actionFullscreen);
        menuMesh->addAction(actionMeshTetraStuffing);
        menuMesh->addAction(actionQuartetTetraStuffing);
        menuMesh->addAction(actionCGAL_Tetrahedralize);
        menuMesh->addAction(actionGenerate_Octree);
        menuMesh->addAction(actionTetGen);
        menuMesh->addAction(actionScale_x10);
        menuMesh->addAction(actionScale_x_0_1);

        retranslateUi(MainWindow);
        QObject::connect(actionQuit, SIGNAL(triggered()), MainWindow, SLOT(close()));
        QObject::connect(actionFullscreen, SIGNAL(toggled(bool)), MainWindow, SLOT(toggleFullScreen(bool)));
        QObject::connect(actionShow_Axis, SIGNAL(toggled(bool)), viewer, SLOT(setAxisIsDrawn(bool)));
        QObject::connect(actionShow_FPS, SIGNAL(toggled(bool)), viewer, SLOT(setFPSIsDisplayed(bool)));
        QObject::connect(actionShow_Grid, SIGNAL(toggled(bool)), viewer, SLOT(setGridIsDrawn(bool)));
        QObject::connect(actionAbout_QGLViewer, SIGNAL(triggered()), viewer, SLOT(aboutQGLViewer()));
        QObject::connect(surfaceCombo, SIGNAL(currentIndexChanged(int)), viewer, SLOT(ToggleTriangleVis(int)));
        QObject::connect(tetraCombo, SIGNAL(currentIndexChanged(int)), viewer, SLOT(ToggleTetraVis(int)));
        QObject::connect(boundingBoxCombo, SIGNAL(currentIndexChanged(int)), viewer, SLOT(ToggleBBox(int)));
        QObject::connect(actionHelp, SIGNAL(triggered()), viewer, SLOT(help()));
        QObject::connect(actionLoad_GMSH, SIGNAL(triggered()), viewer, SLOT(loadGMSH()));
        QObject::connect(surfaceColorButton, SIGNAL(released()), viewer, SLOT(selectSurfaceColor()));
        QObject::connect(surfaceWireframeColorButton, SIGNAL(released()), viewer, SLOT(selectSurfaceColorWireframe()));
        QObject::connect(tetraSolidColorButton, SIGNAL(released()), viewer, SLOT(selectTetraColor()));
        QObject::connect(tetraWireframeColorButton, SIGNAL(released()), viewer, SLOT(selectTetraColorWireframe()));
        QObject::connect(actionMeshTetraStuffing, SIGNAL(triggered()), MainWindow, SLOT(showTetraStuffingDialog()));
        QObject::connect(actionCGAL_Tetrahedralize, SIGNAL(triggered()), MainWindow, SLOT(showCGALDialog()));
        QObject::connect(actionLoad_Surface_OBJ_OFF_3DS_PLY, SIGNAL(triggered()), viewer, SLOT(loadSurface()));
        QObject::connect(actionScale_x10, SIGNAL(triggered()), viewer, SLOT(scale10()));
        QObject::connect(actionScale_x_0_1, SIGNAL(triggered()), viewer, SLOT(scale01()));
        QObject::connect(actionLoad_Surface_OBJ_OFF_3DS_PLY, SIGNAL(triggered()), MainWindow, SLOT(clearTetraOptions()));
        QObject::connect(actionLoad_GMSH, SIGNAL(triggered()), MainWindow, SLOT(clearTetraOptions()));
        QObject::connect(actionExport_GMSH_2, SIGNAL(triggered()), viewer, SLOT(saveGMSH()));
        QObject::connect(actionExport_Surface, SIGNAL(triggered()), viewer, SLOT(saveSurface()));
        QObject::connect(actionGenerate_Octree, SIGNAL(triggered()), MainWindow, SLOT(generateOctree()));
        QObject::connect(octreeCombo, SIGNAL(currentIndexChanged(int)), viewer, SLOT(ToggleOctreeVis(int)));
        QObject::connect(octreeGenerateButton, SIGNAL(released()), MainWindow, SLOT(generateOctree()));
        QObject::connect(actionExport_Tetgen_2, SIGNAL(triggered()), viewer, SLOT(saveTetgen()));
        QObject::connect(actionQuartetTetraStuffing, SIGNAL(triggered()), MainWindow, SLOT(showQuartetTetraDialog()));
        QObject::connect(cutPlaneSlider, SIGNAL(sliderMoved(int)), viewer, SLOT(SetCutPlane(int)));

        octreeCombo->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "QTetraMesher", nullptr));
        actionLoad_GMSH->setText(QCoreApplication::translate("MainWindow", "Load GMSH (.msh)", nullptr));
        actionLoad_NXU2->setText(QCoreApplication::translate("MainWindow", "Load NXU2 (.xml)", nullptr));
        actionLoad_Tetgen->setText(QCoreApplication::translate("MainWindow", "Load Tetgen (not yet...)", nullptr));
        actionExport_OBJ->setText(QCoreApplication::translate("MainWindow", "Export OBJ", nullptr));
        actionExport_GMSH->setText(QCoreApplication::translate("MainWindow", "Export GMSH", nullptr));
        actionExport_Tetgen->setText(QCoreApplication::translate("MainWindow", "Export Tetgen", nullptr));
        actionQuit->setText(QCoreApplication::translate("MainWindow", "Quit", nullptr));
        actionExport_Surface->setText(QCoreApplication::translate("MainWindow", "Export Surface (.obj/.ply/.off/.stl)", nullptr));
        actionExport_GMSH_2->setText(QCoreApplication::translate("MainWindow", "Export GMSH (.msh)", nullptr));
        actionExport_Tetgen_2->setText(QCoreApplication::translate("MainWindow", "Export Tetgen (.node + .ele)", nullptr));
        actionShow_FPS->setText(QCoreApplication::translate("MainWindow", "Show FPS", nullptr));
        actionShow_Grid->setText(QCoreApplication::translate("MainWindow", "Show Grid", nullptr));
        actionShow_Axis->setText(QCoreApplication::translate("MainWindow", "Show Axis", nullptr));
        actionHelp->setText(QCoreApplication::translate("MainWindow", "Help", nullptr));
        actionAbout->setText(QCoreApplication::translate("MainWindow", "About QTetraMesher", nullptr));
        actionFullscreen->setText(QCoreApplication::translate("MainWindow", "Fullscreen", nullptr));
#if QT_CONFIG(tooltip)
        actionFullscreen->setToolTip(QCoreApplication::translate("MainWindow", "Toggle Fullscreen", nullptr));
#endif // QT_CONFIG(tooltip)
        actionAbout_QGLViewer->setText(QCoreApplication::translate("MainWindow", "About QGLViewer", nullptr));
        actionMeshTetraStuffing->setText(QCoreApplication::translate("MainWindow", "SOFATetraStuffing", nullptr));
        actionTetGen->setText(QCoreApplication::translate("MainWindow", "TetGen", nullptr));
        actionCGAL_Tetrahedralize->setText(QCoreApplication::translate("MainWindow", "CGAL Tetrahedralize", nullptr));
        actionLoad_Surface_OBJ_OFF_3DS_PLY->setText(QCoreApplication::translate("MainWindow", "Load Surface  (.obj/.ply/.off/.stl)", nullptr));
        actionScale_x10->setText(QCoreApplication::translate("MainWindow", "Scale x 10", nullptr));
        actionScale_x_0_1->setText(QCoreApplication::translate("MainWindow", "Scale x 0.1", nullptr));
        actionGenerate_Octree->setText(QCoreApplication::translate("MainWindow", "Generate Octree", nullptr));
        actionQuartetTetraStuffing->setText(QCoreApplication::translate("MainWindow", "Quartet TetraStuffing", nullptr));
        actionUpdate_Z_Slider->setText(QCoreApplication::translate("MainWindow", "Update Z Slider", nullptr));
#if QT_CONFIG(tooltip)
        actionUpdate_Z_Slider->setToolTip(QCoreApplication::translate("MainWindow", "Double Cutplane for Z axis", nullptr));
#endif // QT_CONFIG(tooltip)
        octreeVisLabel->setText(QCoreApplication::translate("MainWindow", "Octree Visualization", nullptr));
        octreeCombo->setItemText(0, QCoreApplication::translate("MainWindow", "Hidden", nullptr));
        octreeCombo->setItemText(1, QCoreApplication::translate("MainWindow", "Visible", nullptr));
        octreeCombo->setItemText(2, QCoreApplication::translate("MainWindow", "Leafs Only", nullptr));

        octreeDepthLabel->setText(QCoreApplication::translate("MainWindow", "Octree Depth", nullptr));
        octreeGenerateButton->setText(QCoreApplication::translate("MainWindow", "Generate", nullptr));
        boundingVisLabel->setText(QCoreApplication::translate("MainWindow", "Bounding Box ", nullptr));
        boundingBoxCombo->setItemText(0, QCoreApplication::translate("MainWindow", "Hidden", nullptr));
        boundingBoxCombo->setItemText(1, QCoreApplication::translate("MainWindow", "BoundingBox", nullptr));
        boundingBoxCombo->setItemText(2, QCoreApplication::translate("MainWindow", "BoundingCube", nullptr));

        surfaceVisLabel->setText(QCoreApplication::translate("MainWindow", "Surface Visualization", nullptr));
        surfaceCombo->setItemText(0, QCoreApplication::translate("MainWindow", "Solid", nullptr));
        surfaceCombo->setItemText(1, QCoreApplication::translate("MainWindow", "Wireframe", nullptr));
        surfaceCombo->setItemText(2, QCoreApplication::translate("MainWindow", "Hidden", nullptr));

        surfaceColorButton->setText(QCoreApplication::translate("MainWindow", "Surface Solid Color", nullptr));
        surfaceWireframeColorButton->setText(QCoreApplication::translate("MainWindow", "Surface Wireframe Color", nullptr));
        tetraVisLabel->setText(QCoreApplication::translate("MainWindow", "TetraMesh Visualization", nullptr));
        tetraCombo->setItemText(0, QCoreApplication::translate("MainWindow", "Solid Tetras", nullptr));
        tetraCombo->setItemText(1, QCoreApplication::translate("MainWindow", "Wireframe", nullptr));
        tetraCombo->setItemText(2, QCoreApplication::translate("MainWindow", "Hidden", nullptr));

        tetraSolidColorButton->setText(QCoreApplication::translate("MainWindow", "Tetras Solid Color", nullptr));
        tetraWireframeColorButton->setText(QCoreApplication::translate("MainWindow", "Tetras Wireframe Color", nullptr));
        cutPlaneLabel->setText(QCoreApplication::translate("MainWindow", "Cut Plane X", nullptr));
        menuFile->setTitle(QCoreApplication::translate("MainWindow", "File", nullptr));
        menuHelp_Info->setTitle(QCoreApplication::translate("MainWindow", "Help/Info", nullptr));
        menuView->setTitle(QCoreApplication::translate("MainWindow", "View", nullptr));
        menuMesh->setTitle(QCoreApplication::translate("MainWindow", "Mesh", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
