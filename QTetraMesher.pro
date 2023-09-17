#-------------------------------------------------
#
# Project created by QtCreator 2012-08-26T14:02:01
#
#-------------------------------------------------

QT += core widgets gui xml opengl

VERSION = 0.8.3

# Define the preprocessor macro to get the application version in our application.
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

greaterThan(QT_MAJOR_VERSION, 5): QT += widgets

TARGET = QTetraMesher
TEMPLATE = app

QGLVIEWER_DIR = $$PWD/../libQGLViewer-2.7.1/QGLViewer
SOFA_DIR = $$PWD/../sofa_git/build/install

CONFIG -= debug_and_release debug_and_release_target

FORMS += mainwindow.ui \
         tetrastuffingdialog.ui \
         CGALTetrahedralizeDialog.ui \
         quartettetrastuffingdialog.ui

SOURCES += src/main.cpp\
        src/mainwindow.cpp\
        src/QGLTetraMesh.cpp\
        src/Timer.cpp \
        src/qgltetraviewer.cpp \
        src/tetrastuffingdialog.cpp \
        src/cgaltetrahedralizedialog.cpp \
        src/BaseTetraDialog.cpp \
        src/quartettetrahedralizedialog.cpp \
        src/RangeSlider.cpp

HEADERS  += src/mainwindow.h\
            src/qgltetraviewer.h\
            src/QGLTetraMesh.h\
            src/Timer.h \
            src/tetrastuffingdialog.h \
            src/cgaltetrahedralizedialog.h \
            src/BaseTetraDialog.h \
            src/quartetterahedralizedialog.h \
            src/RangeSlider.h

INCLUDEPATH += $$PWD/src /usr/include/qt
INCLUDEPATH += $$PWD/TetraMeshTools/include $$PWD/include $$PWD/SofaTetraStuffing/include $$PWD/CGALTetrahedralize/include $$PWD/quartet_tetrastuffing/include $$PWD/../sofa/include

win32:CONFIG(release, debug|release): LIBS += -L$$SOFA_DIR/lib -L$$PWD/SofaTetraStuffing/lib -L$$PWD/CGALTetrahedralize/lib -L$$PWD/TetraMeshTools/lib -L$$PWD/quartet_tetrastuffing/lib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$SOFA_DIR/lib -L$$PWD/SofaTetraStuffing/lib -L$$PWD/CGALTetrahedralize/lib -L$$PWD/TetraMeshTools/lib -L$$PWD/quartet_tetrastuffing/lib -L$$QGLVIEWER_DIR
else:unix: LIBS += -L/usr/lib -L$$SOFA_DIR//lib  -lQGLViewer \
                    -L$$SOFA_DIR/include/Sofa.Component.Engine.Generate \
                    -L$$SOFA_DIR/include/Sofa.Config \
                    -L$$SOFA_DIR/include/Sofa.Type \
                    -L$$SOFA_DIR/include/Sofa.DefaultType \
                    -L$$SOFA_DIR/include/Sofa.LinearAlgebra \
                    -L$$SOFA_DIR/include/Sofa.Helper \
                    -L$$SOFA_DIR/include/Sofa.Core \
                    -L$$SOFA_DIR/include/Sofa.Topology \
                    -L$$SOFA_DIR/include/Sofa.Geometry\

win32:CONFIG(release, debug|release): INCLUDEPATH += $$PWD/../sofa/include
else:win32:CONFIG(debug, debug|release): DEPENDPATH += $$PWD/../sofa/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib -L$$PWD/TetraMeshTools/release -lTetraMeshTools -lSofaTetraStuffing -lCGALTetrahedralize -lQGLViewer -lopengl32 -lQuartetTetraStuffing
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib -L$$PWD/TetraMeshTools/debug/ -lTetraMeshToolsd -lSofaTetraStuffingd -lCGALTetrahedralized -lQGLViewerd2 -lopengl32 -lQuartetTetraStuffingd
else:unix: LIBS += -L$$PWD/TetraMeshTools/lib \
                   -L$$PWD/SofaTetraStuffing/lib \
                   -L$$PWD/../sofa/lib \
                   -L$$PWD/CGALTetrahedralize/lib \
                   -L$$PWD/quartet_tetrastuffing/lib \
                   -lCGALTetrahedralize \
                   -lTetraMeshTools \
                   -lSofaTetraStuffing \
                   -lQuartetTetraStuffing \
                   -lSofa.Core \
                   -lSofa.Component.Engine.Generate \
                   -lGLEW \
                   -licui18n \
                   -licuuc \
                   -licudata \
                   -lassimp

