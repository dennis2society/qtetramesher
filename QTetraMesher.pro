#-------------------------------------------------
#
# Project created by QtCreator 2012-08-26T14:02:01
#
#-------------------------------------------------

QT       += core gui xml opengl widgets

VERSION = 0.8.1

# Define the preprocessor macro to get the application version in our application.
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QTetraMesher
TEMPLATE = app

CONFIG -= debug_and_release debug_and_release_target

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

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../sofa/lib -L$$PWD/SofaTetraStuffing/lib -L$$PWD/CGALTetrahedralize/lib -L$$PWD/TetraMeshTools/lib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../sofa/lib -L$$PWD/SofaTetraStuffing/lib -L$$PWD/CGALTetrahedralize/lib -L$$PWD/TetraMeshTools/lib
else:unix: LIBS += -L/usr/lib -L$$PWD/../sofa/lib  -lQGLViewer

win32:CONFIG(release, debug|release): INCLUDEPATH += $$PWD/../sofa/include
else:win32:CONFIG(debug, debug|release): DEPENDPATH += $$PWD/../sofa/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib -L$$PWD/TetraMeshTools/release -lTetraMeshTools -lSofaTetraStuffing -lCGALTetrahedralize -lQGLViewer -lopengl32
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib -L$$PWD/TetraMeshTools/debug/ -lTetraMeshToolsd -lSofaTetraStuffingd -lCGALTetrahedralized -lQGLViewer -lopengl32
else:unix: LIBS += -L$$PWD/TetraMeshTools/lib -L$$PWD/SofaTetraStuffing/lib -L$$PWD/../sofa/lib -L$$PWD/CGALTetrahedralize/lib -lCGALTetrahedralize -lTetraMeshTools -lSofaTetraStuffing -lSofaMisc -lSofaComponentMisc -lSofaCore -lSofaHelper -lSofaDefaultType -lSofaMeshCollision -lSofaDeformable -lSofaObjectInteraction -lSofaBaseCollision -lSofaBaseMechanics -lSofaBaseTopology -lSofaSimulationCommon -lGLEW -lSofaRigid -licui18n -licuuc -licudata 

INCLUDEPATH += $$PWD/TetraMeshTools/include $$PWD/include $$PWD/SofaTetraStuffing/include $$PWD/CGALTetrahedralize/include $$PWD/../sofa/include
DEPENDPATH += $$PWD/TetraMeshTools/include $$PWD/include $$PWD/SofaTetraStuffing/include $$PWD/CGALTetrahedralize/include $$PWD/../sofa/include
