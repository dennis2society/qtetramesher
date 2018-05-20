/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.9.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
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
    QFrame *surfaceVisFrame;
    QHBoxLayout *horizontalLayout_2;
    QVBoxLayout *surfaceVisLayout;
    QLabel *surfaceVisLabel;
    QComboBox *surfaceCombo;
    QPushButton *surfaceColorButton;
    QPushButton *surfaceWireframeColorButton;
    QFrame *boundingBoxFrame;
    QHBoxLayout *horizontalLayout_4;
    QVBoxLayout *verticalLayout_2;
    QLabel *boundingVisLabel;
    QComboBox *boundingBoxCombo;
    QFrame *octreeFrame;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *octreeVisLayout;
    QLabel *octreeVisLabel;
    QComboBox *octreeCombo;
    QLabel *octreeDepthLabel;
    QHBoxLayout *horizontalLayout_5;
    QSpinBox *octreeDepthSpinbox;
    QPushButton *octreeGenerateButton;
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
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(1024, 828);
        QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        sizePolicy.setHorizontalStretch(1);
        sizePolicy.setVerticalStretch(1);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        MainWindow->setMinimumSize(QSize(0, 0));
        actionLoad_GMSH = new QAction(MainWindow);
        actionLoad_GMSH->setObjectName(QStringLiteral("actionLoad_GMSH"));
        actionLoad_NXU2 = new QAction(MainWindow);
        actionLoad_NXU2->setObjectName(QStringLiteral("actionLoad_NXU2"));
        actionLoad_Tetgen = new QAction(MainWindow);
        actionLoad_Tetgen->setObjectName(QStringLiteral("actionLoad_Tetgen"));
        actionExport_OBJ = new QAction(MainWindow);
        actionExport_OBJ->setObjectName(QStringLiteral("actionExport_OBJ"));
        actionExport_GMSH = new QAction(MainWindow);
        actionExport_GMSH->setObjectName(QStringLiteral("actionExport_GMSH"));
        actionExport_Tetgen = new QAction(MainWindow);
        actionExport_Tetgen->setObjectName(QStringLiteral("actionExport_Tetgen"));
        actionQuit = new QAction(MainWindow);
        actionQuit->setObjectName(QStringLiteral("actionQuit"));
        actionExport_Surface = new QAction(MainWindow);
        actionExport_Surface->setObjectName(QStringLiteral("actionExport_Surface"));
        actionExport_GMSH_2 = new QAction(MainWindow);
        actionExport_GMSH_2->setObjectName(QStringLiteral("actionExport_GMSH_2"));
        actionExport_Tetgen_2 = new QAction(MainWindow);
        actionExport_Tetgen_2->setObjectName(QStringLiteral("actionExport_Tetgen_2"));
        actionShow_FPS = new QAction(MainWindow);
        actionShow_FPS->setObjectName(QStringLiteral("actionShow_FPS"));
        actionShow_FPS->setCheckable(true);
        actionShow_FPS->setChecked(true);
        actionShow_Grid = new QAction(MainWindow);
        actionShow_Grid->setObjectName(QStringLiteral("actionShow_Grid"));
        actionShow_Grid->setCheckable(true);
        actionShow_Grid->setChecked(true);
        actionShow_Axis = new QAction(MainWindow);
        actionShow_Axis->setObjectName(QStringLiteral("actionShow_Axis"));
        actionShow_Axis->setCheckable(true);
        actionShow_Axis->setChecked(true);
        actionHelp = new QAction(MainWindow);
        actionHelp->setObjectName(QStringLiteral("actionHelp"));
        actionAbout = new QAction(MainWindow);
        actionAbout->setObjectName(QStringLiteral("actionAbout"));
        actionFullscreen = new QAction(MainWindow);
        actionFullscreen->setObjectName(QStringLiteral("actionFullscreen"));
        actionFullscreen->setCheckable(true);
        actionAbout_QGLViewer = new QAction(MainWindow);
        actionAbout_QGLViewer->setObjectName(QStringLiteral("actionAbout_QGLViewer"));
        actionMeshTetraStuffing = new QAction(MainWindow);
        actionMeshTetraStuffing->setObjectName(QStringLiteral("actionMeshTetraStuffing"));
        actionTetGen = new QAction(MainWindow);
        actionTetGen->setObjectName(QStringLiteral("actionTetGen"));
        actionCGAL_Tetrahedralize = new QAction(MainWindow);
        actionCGAL_Tetrahedralize->setObjectName(QStringLiteral("actionCGAL_Tetrahedralize"));
        actionLoad_Surface_OBJ_OFF_3DS_PLY = new QAction(MainWindow);
        actionLoad_Surface_OBJ_OFF_3DS_PLY->setObjectName(QStringLiteral("actionLoad_Surface_OBJ_OFF_3DS_PLY"));
        actionScale_x10 = new QAction(MainWindow);
        actionScale_x10->setObjectName(QStringLiteral("actionScale_x10"));
        actionScale_x_0_1 = new QAction(MainWindow);
        actionScale_x_0_1->setObjectName(QStringLiteral("actionScale_x_0_1"));
        actionGenerate_Octree = new QAction(MainWindow);
        actionGenerate_Octree->setObjectName(QStringLiteral("actionGenerate_Octree"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
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
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(2, 0, 1, 1);
        mainLayout = new QHBoxLayout();
        mainLayout->setSpacing(1);
        mainLayout->setObjectName(QStringLiteral("mainLayout"));
        viewAndControlFrame = new QFrame(centralWidget);
        viewAndControlFrame->setObjectName(QStringLiteral("viewAndControlFrame"));
        viewAndControlFrame->setFrameShape(QFrame::Box);
        viewAndControlFrame->setFrameShadow(QFrame::Raised);
        gridLayout_3 = new QGridLayout(viewAndControlFrame);
        gridLayout_3->setSpacing(1);
        gridLayout_3->setContentsMargins(1, 1, 1, 1);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        viewAndControlLayout = new QVBoxLayout();
        viewAndControlLayout->setSpacing(1);
        viewAndControlLayout->setObjectName(QStringLiteral("viewAndControlLayout"));
        qglViewerFrame = new QFrame(viewAndControlFrame);
        qglViewerFrame->setObjectName(QStringLiteral("qglViewerFrame"));
        qglViewerFrame->setFrameShape(QFrame::Box);
        qglViewerFrame->setFrameShadow(QFrame::Raised);
        gridLayout_4 = new QGridLayout(qglViewerFrame);
        gridLayout_4->setSpacing(1);
        gridLayout_4->setContentsMargins(1, 1, 1, 1);
        gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));
        viewer = new QGLTetraViewer(qglViewerFrame);
        viewer->setObjectName(QStringLiteral("viewer"));

        gridLayout_4->addWidget(viewer, 0, 0, 1, 1);


        viewAndControlLayout->addWidget(qglViewerFrame);


        gridLayout_3->addLayout(viewAndControlLayout, 0, 0, 1, 1);


        mainLayout->addWidget(viewAndControlFrame);

        visualizationFrame = new QFrame(centralWidget);
        visualizationFrame->setObjectName(QStringLiteral("visualizationFrame"));
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
        horizontalLayout_3->setContentsMargins(1, 1, 1, 1);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        gridLayout_2 = new QGridLayout();
        gridLayout_2->setSpacing(1);
        gridLayout_2->setContentsMargins(1, 1, 1, 1);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        surfaceVisFrame = new QFrame(visualizationFrame);
        surfaceVisFrame->setObjectName(QStringLiteral("surfaceVisFrame"));
        QSizePolicy sizePolicy3(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(surfaceVisFrame->sizePolicy().hasHeightForWidth());
        surfaceVisFrame->setSizePolicy(sizePolicy3);
        surfaceVisFrame->setMinimumSize(QSize(0, 100));
        surfaceVisFrame->setFrameShape(QFrame::Box);
        surfaceVisFrame->setFrameShadow(QFrame::Sunken);
        horizontalLayout_2 = new QHBoxLayout(surfaceVisFrame);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(2, 2, 2, 2);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        surfaceVisLayout = new QVBoxLayout();
        surfaceVisLayout->setSpacing(1);
        surfaceVisLayout->setContentsMargins(1, 1, 1, 1);
        surfaceVisLayout->setObjectName(QStringLiteral("surfaceVisLayout"));
        surfaceVisLabel = new QLabel(surfaceVisFrame);
        surfaceVisLabel->setObjectName(QStringLiteral("surfaceVisLabel"));
        QFont font;
        font.setPointSize(12);
        font.setBold(true);
        font.setWeight(75);
        surfaceVisLabel->setFont(font);
        surfaceVisLabel->setFrameShape(QFrame::NoFrame);
        surfaceVisLabel->setAlignment(Qt::AlignCenter);
        surfaceVisLabel->setMargin(2);

        surfaceVisLayout->addWidget(surfaceVisLabel);

        surfaceCombo = new QComboBox(surfaceVisFrame);
        surfaceCombo->setObjectName(QStringLiteral("surfaceCombo"));
        surfaceCombo->setMinimumSize(QSize(0, 30));
        QFont font1;
        font1.setPointSize(10);
        font1.setBold(true);
        font1.setWeight(75);
        surfaceCombo->setFont(font1);

        surfaceVisLayout->addWidget(surfaceCombo);

        surfaceColorButton = new QPushButton(surfaceVisFrame);
        surfaceColorButton->setObjectName(QStringLiteral("surfaceColorButton"));

        surfaceVisLayout->addWidget(surfaceColorButton);

        surfaceWireframeColorButton = new QPushButton(surfaceVisFrame);
        surfaceWireframeColorButton->setObjectName(QStringLiteral("surfaceWireframeColorButton"));

        surfaceVisLayout->addWidget(surfaceWireframeColorButton);


        horizontalLayout_2->addLayout(surfaceVisLayout);


        gridLayout_2->addWidget(surfaceVisFrame, 1, 0, 1, 1);

        boundingBoxFrame = new QFrame(visualizationFrame);
        boundingBoxFrame->setObjectName(QStringLiteral("boundingBoxFrame"));
        sizePolicy3.setHeightForWidth(boundingBoxFrame->sizePolicy().hasHeightForWidth());
        boundingBoxFrame->setSizePolicy(sizePolicy3);
        boundingBoxFrame->setMinimumSize(QSize(0, 100));
        boundingBoxFrame->setFrameShape(QFrame::Box);
        boundingBoxFrame->setFrameShadow(QFrame::Sunken);
        horizontalLayout_4 = new QHBoxLayout(boundingBoxFrame);
        horizontalLayout_4->setSpacing(1);
        horizontalLayout_4->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(1, 1, 1, -1);
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(1);
        verticalLayout_2->setContentsMargins(1, 1, 1, 1);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        boundingVisLabel = new QLabel(boundingBoxFrame);
        boundingVisLabel->setObjectName(QStringLiteral("boundingVisLabel"));
        boundingVisLabel->setFont(font);
        boundingVisLabel->setAlignment(Qt::AlignCenter);

        verticalLayout_2->addWidget(boundingVisLabel);

        boundingBoxCombo = new QComboBox(boundingBoxFrame);
        boundingBoxCombo->setObjectName(QStringLiteral("boundingBoxCombo"));
        boundingBoxCombo->setMinimumSize(QSize(0, 30));
        boundingBoxCombo->setFont(font1);

        verticalLayout_2->addWidget(boundingBoxCombo);


        horizontalLayout_4->addLayout(verticalLayout_2);


        gridLayout_2->addWidget(boundingBoxFrame, 4, 0, 1, 1);

        octreeFrame = new QFrame(visualizationFrame);
        octreeFrame->setObjectName(QStringLiteral("octreeFrame"));
        sizePolicy3.setHeightForWidth(octreeFrame->sizePolicy().hasHeightForWidth());
        octreeFrame->setSizePolicy(sizePolicy3);
        octreeFrame->setMinimumSize(QSize(0, 120));
        octreeFrame->setFrameShape(QFrame::Box);
        octreeFrame->setFrameShadow(QFrame::Sunken);
        verticalLayoutWidget = new QWidget(octreeFrame);
        verticalLayoutWidget->setObjectName(QStringLiteral("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(0, 0, 201, 112));
        octreeVisLayout = new QVBoxLayout(verticalLayoutWidget);
        octreeVisLayout->setSpacing(1);
        octreeVisLayout->setContentsMargins(1, 1, 1, 1);
        octreeVisLayout->setObjectName(QStringLiteral("octreeVisLayout"));
        octreeVisLayout->setContentsMargins(0, 0, 0, 0);
        octreeVisLabel = new QLabel(verticalLayoutWidget);
        octreeVisLabel->setObjectName(QStringLiteral("octreeVisLabel"));
        sizePolicy3.setHeightForWidth(octreeVisLabel->sizePolicy().hasHeightForWidth());
        octreeVisLabel->setSizePolicy(sizePolicy3);
        octreeVisLabel->setMinimumSize(QSize(0, 30));
        octreeVisLabel->setFont(font);
        octreeVisLabel->setAlignment(Qt::AlignCenter);
        octreeVisLabel->setMargin(2);

        octreeVisLayout->addWidget(octreeVisLabel);

        octreeCombo = new QComboBox(verticalLayoutWidget);
        octreeCombo->setObjectName(QStringLiteral("octreeCombo"));
        sizePolicy3.setHeightForWidth(octreeCombo->sizePolicy().hasHeightForWidth());
        octreeCombo->setSizePolicy(sizePolicy3);
        octreeCombo->setMinimumSize(QSize(0, 30));
        octreeCombo->setFont(font1);
        octreeCombo->setFrame(true);

        octreeVisLayout->addWidget(octreeCombo);

        octreeDepthLabel = new QLabel(verticalLayoutWidget);
        octreeDepthLabel->setObjectName(QStringLiteral("octreeDepthLabel"));
        QFont font2;
        font2.setBold(true);
        font2.setWeight(75);
        octreeDepthLabel->setFont(font2);

        octreeVisLayout->addWidget(octreeDepthLabel);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        octreeDepthSpinbox = new QSpinBox(verticalLayoutWidget);
        octreeDepthSpinbox->setObjectName(QStringLiteral("octreeDepthSpinbox"));
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
        octreeGenerateButton->setObjectName(QStringLiteral("octreeGenerateButton"));
        octreeGenerateButton->setFont(font2);

        horizontalLayout_5->addWidget(octreeGenerateButton);


        octreeVisLayout->addLayout(horizontalLayout_5);


        gridLayout_2->addWidget(octreeFrame, 5, 0, 1, 1);

        tetraVisFrame = new QFrame(visualizationFrame);
        tetraVisFrame->setObjectName(QStringLiteral("tetraVisFrame"));
        sizePolicy3.setHeightForWidth(tetraVisFrame->sizePolicy().hasHeightForWidth());
        tetraVisFrame->setSizePolicy(sizePolicy3);
        tetraVisFrame->setMinimumSize(QSize(0, 200));
        tetraVisFrame->setFrameShape(QFrame::Box);
        tetraVisFrame->setFrameShadow(QFrame::Sunken);
        horizontalLayout = new QHBoxLayout(tetraVisFrame);
        horizontalLayout->setSpacing(1);
        horizontalLayout->setContentsMargins(1, 1, 1, 1);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(1);
        verticalLayout->setContentsMargins(1, 1, 1, 1);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        tetraVisLabel = new QLabel(tetraVisFrame);
        tetraVisLabel->setObjectName(QStringLiteral("tetraVisLabel"));
        tetraVisLabel->setFont(font);
        tetraVisLabel->setTextFormat(Qt::AutoText);
        tetraVisLabel->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(tetraVisLabel);

        tetraCombo = new QComboBox(tetraVisFrame);
        tetraCombo->setObjectName(QStringLiteral("tetraCombo"));
        tetraCombo->setMinimumSize(QSize(0, 30));
        tetraCombo->setFont(font1);

        verticalLayout->addWidget(tetraCombo);

        tetraSolidColorButton = new QPushButton(tetraVisFrame);
        tetraSolidColorButton->setObjectName(QStringLiteral("tetraSolidColorButton"));

        verticalLayout->addWidget(tetraSolidColorButton);

        tetraWireframeColorButton = new QPushButton(tetraVisFrame);
        tetraWireframeColorButton->setObjectName(QStringLiteral("tetraWireframeColorButton"));

        verticalLayout->addWidget(tetraWireframeColorButton);

        line = new QFrame(tetraVisFrame);
        line->setObjectName(QStringLiteral("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line);

        cutPlaneLabel = new QLabel(tetraVisFrame);
        cutPlaneLabel->setObjectName(QStringLiteral("cutPlaneLabel"));
        QFont font3;
        font3.setPointSize(11);
        font3.setBold(true);
        font3.setWeight(75);
        cutPlaneLabel->setFont(font3);
        cutPlaneLabel->setFrameShape(QFrame::NoFrame);
        cutPlaneLabel->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        verticalLayout->addWidget(cutPlaneLabel);

        cutPlaneSlider = new QSlider(tetraVisFrame);
        cutPlaneSlider->setObjectName(QStringLiteral("cutPlaneSlider"));
        cutPlaneSlider->setMinimumSize(QSize(0, 30));
        cutPlaneSlider->setOrientation(Qt::Horizontal);
        cutPlaneSlider->setInvertedAppearance(false);
        cutPlaneSlider->setInvertedControls(true);

        verticalLayout->addWidget(cutPlaneSlider);


        horizontalLayout->addLayout(verticalLayout);


        gridLayout_2->addWidget(tetraVisFrame, 2, 0, 1, 1);


        horizontalLayout_3->addLayout(gridLayout_2);


        mainLayout->addWidget(visualizationFrame);


        gridLayout->addLayout(mainLayout, 0, 1, 1, 1);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1024, 21));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        menuHelp_Info = new QMenu(menuBar);
        menuHelp_Info->setObjectName(QStringLiteral("menuHelp_Info"));
        menuView = new QMenu(menuBar);
        menuView->setObjectName(QStringLiteral("menuView"));
        menuMesh = new QMenu(menuBar);
        menuMesh->setObjectName(QStringLiteral("menuMesh"));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

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
        QObject::connect(cutPlaneSlider, SIGNAL(sliderMoved(int)), viewer, SLOT(SetCutPlane(int)));
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

        octreeCombo->setCurrentIndex(1);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "QTetraMesher", Q_NULLPTR));
        actionLoad_GMSH->setText(QApplication::translate("MainWindow", "Load GMSH (.msh)", Q_NULLPTR));
        actionLoad_NXU2->setText(QApplication::translate("MainWindow", "Load NXU2 (.xml)", Q_NULLPTR));
        actionLoad_Tetgen->setText(QApplication::translate("MainWindow", "Load Tetgen (not yet...)", Q_NULLPTR));
        actionExport_OBJ->setText(QApplication::translate("MainWindow", "Export OBJ", Q_NULLPTR));
        actionExport_GMSH->setText(QApplication::translate("MainWindow", "Export GMSH", Q_NULLPTR));
        actionExport_Tetgen->setText(QApplication::translate("MainWindow", "Export Tetgen", Q_NULLPTR));
        actionQuit->setText(QApplication::translate("MainWindow", "Quit", Q_NULLPTR));
        actionExport_Surface->setText(QApplication::translate("MainWindow", "Export Surface (.obj/.ply/.off/.stl)", Q_NULLPTR));
        actionExport_GMSH_2->setText(QApplication::translate("MainWindow", "Export GMSH (.msh)", Q_NULLPTR));
        actionExport_Tetgen_2->setText(QApplication::translate("MainWindow", "Export Tetgen (.node + .ele)", Q_NULLPTR));
        actionShow_FPS->setText(QApplication::translate("MainWindow", "Show FPS", Q_NULLPTR));
        actionShow_Grid->setText(QApplication::translate("MainWindow", "Show Grid", Q_NULLPTR));
        actionShow_Axis->setText(QApplication::translate("MainWindow", "Show Axis", Q_NULLPTR));
        actionHelp->setText(QApplication::translate("MainWindow", "Help", Q_NULLPTR));
        actionAbout->setText(QApplication::translate("MainWindow", "About QTetraMesher", Q_NULLPTR));
        actionFullscreen->setText(QApplication::translate("MainWindow", "Fullscreen", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        actionFullscreen->setToolTip(QApplication::translate("MainWindow", "Toggle Fullscreen", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        actionAbout_QGLViewer->setText(QApplication::translate("MainWindow", "About QGLViewer", Q_NULLPTR));
        actionMeshTetraStuffing->setText(QApplication::translate("MainWindow", "MeshTetraStuffing", Q_NULLPTR));
        actionTetGen->setText(QApplication::translate("MainWindow", "TetGen", Q_NULLPTR));
        actionCGAL_Tetrahedralize->setText(QApplication::translate("MainWindow", "CGAL Tetrahedralize", Q_NULLPTR));
        actionLoad_Surface_OBJ_OFF_3DS_PLY->setText(QApplication::translate("MainWindow", "Load Surface  (.obj/.ply/.off/.stl)", Q_NULLPTR));
        actionScale_x10->setText(QApplication::translate("MainWindow", "Scale x 10", Q_NULLPTR));
        actionScale_x_0_1->setText(QApplication::translate("MainWindow", "Scale x 0.1", Q_NULLPTR));
        actionGenerate_Octree->setText(QApplication::translate("MainWindow", "Generate Octree", Q_NULLPTR));
        surfaceVisLabel->setText(QApplication::translate("MainWindow", "Surface Visualization", Q_NULLPTR));
        surfaceCombo->clear();
        surfaceCombo->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "Solid", Q_NULLPTR)
         << QApplication::translate("MainWindow", "Wireframe", Q_NULLPTR)
         << QApplication::translate("MainWindow", "Hidden", Q_NULLPTR)
        );
        surfaceColorButton->setText(QApplication::translate("MainWindow", "Surface Solid Color", Q_NULLPTR));
        surfaceWireframeColorButton->setText(QApplication::translate("MainWindow", "Surface Wireframe Color", Q_NULLPTR));
        boundingVisLabel->setText(QApplication::translate("MainWindow", "Bounding Box ", Q_NULLPTR));
        boundingBoxCombo->clear();
        boundingBoxCombo->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "Hidden", Q_NULLPTR)
         << QApplication::translate("MainWindow", "BoundingBox", Q_NULLPTR)
         << QApplication::translate("MainWindow", "BoundingCube", Q_NULLPTR)
        );
        octreeVisLabel->setText(QApplication::translate("MainWindow", "Octree Visualization", Q_NULLPTR));
        octreeCombo->clear();
        octreeCombo->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "Hidden", Q_NULLPTR)
         << QApplication::translate("MainWindow", "Visible", Q_NULLPTR)
         << QApplication::translate("MainWindow", "Leafs Only", Q_NULLPTR)
        );
        octreeDepthLabel->setText(QApplication::translate("MainWindow", "Octree Depth", Q_NULLPTR));
        octreeGenerateButton->setText(QApplication::translate("MainWindow", "Generate", Q_NULLPTR));
        tetraVisLabel->setText(QApplication::translate("MainWindow", "TetraMesh Visualization", Q_NULLPTR));
        tetraCombo->clear();
        tetraCombo->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "Solid Tetras", Q_NULLPTR)
         << QApplication::translate("MainWindow", "Wireframe", Q_NULLPTR)
         << QApplication::translate("MainWindow", "Hidden", Q_NULLPTR)
        );
        tetraSolidColorButton->setText(QApplication::translate("MainWindow", "Tetras Solid Color", Q_NULLPTR));
        tetraWireframeColorButton->setText(QApplication::translate("MainWindow", "Tetras Wireframe Color", Q_NULLPTR));
        cutPlaneLabel->setText(QApplication::translate("MainWindow", "Cut Plane", Q_NULLPTR));
        menuFile->setTitle(QApplication::translate("MainWindow", "File", Q_NULLPTR));
        menuHelp_Info->setTitle(QApplication::translate("MainWindow", "Help/Info", Q_NULLPTR));
        menuView->setTitle(QApplication::translate("MainWindow", "View", Q_NULLPTR));
        menuMesh->setTitle(QApplication::translate("MainWindow", "Mesh", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
