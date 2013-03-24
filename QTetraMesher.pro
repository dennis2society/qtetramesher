#-------------------------------------------------
#
# Project created by QtCreator 2012-08-26T14:02:01
#
#-------------------------------------------------

QT       += core gui xml opengl

VERSION = 0.8.1

# Define the preprocessor macro to get the application version in our application.
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QTetraMesher
TEMPLATE = app


SOURCES += src/main.cpp\
        src/mainwindow.cpp\
        src/QGLTetraMesh.cpp\
        src/Timer.cpp \
    src/qgltetraviewer.cpp \
    src/tetrastuffingdialog.cpp \
    src/cgaltetrahedralizedialog.cpp \
    src/BaseTetraDialog.cpp

HEADERS  += src/mainwindow.h\
            src/qgltetraviewer.h\
            src/QGLTetraMesh.h\
            src/Timer.h \
    src/tetrastuffingdialog.h \
    src/cgaltetrahedralizedialog.h \
    src/BaseTetraDialog.h

FORMS    += mainwindow.ui \
    tetrastuffingdialog.ui \
    CGALTetrahedralizeDialog.ui \
    mainwindow.ui

INCLUDEPATH += $$PWD/src

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../libQGLViewer-2.3.17/QGLViewer/release/ -lQGLViewer2
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../libQGLViewer-2.3.17/QGLViewer/debug/ -lQGLViewerd2
else:unix: LIBS += -L/usr/lib/i386-linux-gnu -lqglviewer-qt4

INCLUDEPATH += $$PWD/../../libQGLViewer-2.3.17
DEPENDPATH += $$PWD/../../libQGLViewer-2.3.17

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib -L$$PWD/TetraMeshTools/release -lTetraMeshTools -lSofaTetraStuffing -lCGALTetrahedralize
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib -L$$PWD/TetraMeshTools/debug/ -lTetraMeshToolsd -lSofaTetraStuffingd -lCGALTetrahedralize
else:unix: LIBS += -L$$PWD/TetraMeshTools/lib -L$$PWD/SofaTetraStuffing/lib -L$$PWD/../sofa/lib -L$$PWD/../sofa/extlibs -L$$PWD/CGALTetrahedralize/lib -lCGALTetrahedralize -lTetraMeshTools -lSofaTetraStuffing -lsofa_misc -lsofa_component_misc -lsofacore -lsofahelper -lsofadefaulttype -lsofa_mesh_collision -lsofa_deformable -lsofa_object_interaction -lsofa_base_collision -lsofa_base_mechanics -lsofa_base_topology -lsofasimulation -lGLEW -ltinyxml -lsofa_rigid

INCLUDEPATH += $$PWD/TetraMeshTools/include $$PWD/include $$PWD/SofaTetraStuffing/include $$PWD/CGALTetrahedralize/include
DEPENDPATH += $$PWD/TetraMeshTools/include $$PWD/include $$PWD/SofaTetraStuffing/include $$PWD/CGALTetrahedralize/include
