#-------------------------------------------------
#
# Project created by QtCreator 2018-06-02T14:47:47
#
#-------------------------------------------------

QT       -= core gui

TARGET = CGALTetrahedralize
TEMPLATE = lib

DEFINES += CGAL_TETRAHEDRALIZE_LIBRARY

# only needed for Windows compilation, adapt to your own paths!
CGAL_DIR = $$PWD/../../cgal_4_12
BOOST_DIR = $$PWD/../../boost_1_64
EIGEN_DIR = $$PWD/../../eigen3

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

INCLUDEPATH += $$PWD/include  $$PWD/../TetraMeshTools/include $$SOFA_DIR/include

unix: LIBS += -L/usr/lib -L$$SOFA_DIR/lib -lboost_thread -lgmp -lmpfr

unix {
    target.path = /usr/lib
    INSTALLS += target
}

win32 {
    INCLUDEPATH += $$BOOST_DIR $$CGAL_DIR/include $$BOOST_DIR $$CGAL_DIR/build/include $$EIGEN_DIR $$CGAL_DIR/auxiliary/gmp/include
    LIBS += -L$$CGAL_DIR/build/lib -L$$CGAL_DIR/auxiliary/gmp/lib
}

# add trailing "d" to TARGET name for DEBUG build
win32:CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
    QMAKE_CXXFLAGS += -bigobj
    # adapt next line to your local verions of CGAL_Core, CGAL, gmp and mpfr
    LIBS += -lCGAL_Core-vc140-mt-gd-4.12 -lCGAL-vc140-mt-gd-4.12 -llibgmp-10 -llibmpfr-4
}

win32:CONFIG(release, debug|release) {
    # adapt next line to your local verions of CGAL_Core, CGAL, gmp and mpfr
    LIBS += -lCGAL_Core-vc140-mt-4.12 -lCGAL-vc140-mt-4.12 -llibgmp-10 -llibmpfr-4
}



HEADERS += \
    include/CGALTetrahedralize.h

SOURCES += \
    src/CGALTetrahedralize.cpp

