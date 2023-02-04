#-------------------------------------------------
#
# Project created by QtCreator 2018-06-02T14:47:47
#
#-------------------------------------------------

QT       -= core

TARGET = SofaTetraStuffing
TEMPLATE = lib

DEFINES += SOFA_TETRASTUFFING_LIBRARY

# you may want to change this to your SOFA installation path
SOFA_DIR = $$PWD/../../sofa
BOOST_DIR = $$PWD/../../boost_1_64

# use fixed output folder and disable QtCreator's DEBUG/RELEASE and intermediate folders
CONFIG -= debug_and_release debug_and_release_target

# disable symlinks with version numbers (e.g. don't create symlink for lib*.so to lib*.so.1.0.0)
QMAKE_LN_SHLIB       = :
# disable *.so version numbers (only output *.so instead of *.so.1.0.0)
CONFIG += plugin

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#INCLUDEPATH += $$PWD/src $$PWD/include  $$PWD/../TetraMeshTools/include $$SOFA_DIR/include/SofaMisc $$SOFA_DIR/include/SofaFramework
INCLUDEPATH += $$PWD/src $$PWD/include \
               $$PWD/../TetraMeshTools/include \
               $$SOFA_DIR/include/Sofa.Component.Engine.Generate \
               $$SOFA_DIR/include/Sofa.Config \
               $$SOFA_DIR/include/Sofa.Type \
               $$SOFA_DIR/include/Sofa.DefaultType \
               $$SOFA_DIR/include/Sofa.LinearAlgebra \
               $$SOFA_DIR/include/Sofa.Helper \
               $$SOFA_DIR/include/Sofa.Core \
               $$SOFA_DIR/include/Sofa.Topology \
               $$SOFA_DIR/include/Sofa.Geometry

unix: LIBS += -L$$SOFA_DIR/lib -L/usr/lib -L$$SOFA_DIR/lib -lSofa.Core


unix {
    target.path = /usr/lib
    INSTALLS += target
}

# add trailing "d" to TARGET name for DEBUG build
win32:CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
}

win32 {
    INCLUDEPATH += $$BOOST_DIR
    LIBS += -L$$SOFA_DIR/lib -lSofaCore
}

HEADERS += \
    include/SofaTetraStuffing.h

SOURCES += \
    src/SofaTetraStuffing.cpp
