#-------------------------------------------------
#
# Project created by QtCreator 2012-08-26T14:02:01
#
#-------------------------------------------------

QT       += core gui xml opengl widgets

VERSION = 0.8.2

# Define the preprocessor macro to get the application version in our application.
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QTetraMesher
TEMPLATE = app

QGLVIEWER_DIR = $$PWD/../libQGLViewer-2.7.1/QGLViewer
SOFA_DIR = $$PWD/../sofa

CONFIG -= debug_and_release debug_and_release_target

SOURCES += src/main.cpp\
        src/mainwindow.cpp\
        src/QGLTetraMesh.cpp\
        src/Timer.cpp \
    src/qgltetraviewer.cpp \
    src/tetrastuffingdialog.cpp \
    src/cgaltetrahedralizedialog.cpp \
    src/BaseTetraDialog.cpp \
    src/quartettetrahedralizedialog.cpp

HEADERS  += src/mainwindow.h\
            src/qgltetraviewer.h\
            src/QGLTetraMesh.h\
            src/Timer.h \
    src/tetrastuffingdialog.h \
    src/cgaltetrahedralizedialog.h \
    src/BaseTetraDialog.h \
    src/quartetterahedralizedialog.h

FORMS    += mainwindow.ui \
    tetrastuffingdialog.ui \
    CGALTetrahedralizeDialog.ui \
    quartettetrastuffingdialog.ui

INCLUDEPATH += $$PWD/src
INCLUDEPATH += $$PWD/TetraMeshTools/include $$PWD/include $$PWD/SofaTetraStuffing/include $$PWD/CGALTetrahedralize/include $$PWD/quartet_tetrastuffing/include $$PWD/../sofa/include

win32:CONFIG(release, debug|release): LIBS += -L$$SOFA_DIR/lib -L$$PWD/SofaTetraStuffing/lib -L$$PWD/CGALTetrahedralize/lib -L$$PWD/TetraMeshTools/lib -L$$PWD/quartet_tetrastuffing/lib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$SOFA_DIR/lib -L$$PWD/SofaTetraStuffing/lib -L$$PWD/CGALTetrahedralize/lib -L$$PWD/TetraMeshTools/lib -L$$PWD/quartet_tetrastuffing/lib -L$$QGLVIEWER_DIR
else:unix: LIBS += -L/usr/lib -L$$PWD/../sofa/lib  -lQGLViewer

win32:CONFIG(release, debug|release): INCLUDEPATH += $$PWD/../sofa/include
else:win32:CONFIG(debug, debug|release): DEPENDPATH += $$PWD/../sofa/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib -L$$PWD/TetraMeshTools/release -lTetraMeshTools -lSofaTetraStuffing -lCGALTetrahedralize -lQGLViewer -lopengl32 -lQuartetTetraStuffing
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib -L$$PWD/TetraMeshTools/debug/ -lTetraMeshToolsd -lSofaTetraStuffingd -lCGALTetrahedralized -lQGLViewerd2 -lopengl32 -lQuartetTetraStuffingd
else:unix: LIBS += -L$$PWD/TetraMeshTools/lib -L$$PWD/SofaTetraStuffing/lib -L$$PWD/../sofa/lib -L$$PWD/CGALTetrahedralize/lib -L$$PWD/quartet_tetrastuffing/lib -lCGALTetrahedralize -lTetraMeshTools -lSofaTetraStuffing -lQuartetTetraStuffing -lSofaMisc -lSofaComponentMisc -lSofaCore -lSofaHelper -lSofaDefaultType -lSofaMeshCollision -lSofaDeformable -lSofaObjectInteraction -lSofaBaseCollision -lSofaBaseMechanics -lSofaBaseTopology -lSofaSimulationCommon -lGLEW -lSofaRigid -licui18n -licuuc -licudata

