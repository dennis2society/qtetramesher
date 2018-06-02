#-------------------------------------------------
#
# Project created by QtCreator 2018-06-02T14:47:47
#
#-------------------------------------------------

QT       -= core gui

TARGET = QuartetTetraStuffing
TEMPLATE = lib

DEFINES += QUARTET_TETRASTUFFING_LIBRARY

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

INCLUDEPATH += $$PWD/src $$PWD/include  $$PWD/../TetraMeshTools/include

SOURCES += \
    src/feature.cpp \
    src/geometry_queries.cpp \
    src/make_signed_distance.cpp \
    src/make_tet_mesh.cpp \
    src/match_features.cpp \
    src/optimize_tet_mesh.cpp \
    src/predicates.cpp \
    src/QuartetTetraStuffing.cpp \
    src/sdf.cpp \
    src/tet_mesh.cpp \
    src/tet_quality.cpp \
    src/trimesh.cpp

HEADERS += \
    include/QuartetTetraStuffing.h \
    src/array3.h \
    src/feature.h \
    src/geometry_queries.h \
    src/make_signed_distance.h \
    src/make_tet_mesh.h \
    src/match_features.h \
    src/optimize_tet_mesh.h \
    src/sdf.h \
    src/tet_mesh.h \
    src/tet_quality.h \
    src/trimesh.h \
    src/util.h \
    src/vec.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
