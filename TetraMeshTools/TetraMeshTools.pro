#-------------------------------------------------
#
# Project created by QtCreator 2018-06-02T14:47:47
#
#-------------------------------------------------

QT       -= core gui

TARGET = TetraMeshTools
TEMPLATE = lib

DEFINES += TETRAMESHTOOLS_LIBRARY

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

QMAKE_CXXFLAGS += -O3 -fopenmp

INCLUDEPATH += $$PWD/src $$PWD/include $$PWD/include/TetraMeshTools $$PWD/trimesh2/include /usr/include

unix {
    target.path = $$PWD/lib
    INSTALLS += target
}

DESTDIR = $$PWD/lib
CONFIG(debug, debug|release) {
    DESTDIR = $$PWD/lib
    DESTDIR += ../debug_linux_x64
} else {
    DESTDIR = $$PWD/lib
    DESTDIR += ../bin_linux_x64
}

# add trailing "d" to TARGET name for DEBUG build
win32:CONFIG(debug, debug|release) {
    TARGET = $$join(TARGET,,,d)
}

HEADERS += \
    include/TetraMeshTools/BaseMeshLoader.h \
    include/TetraMeshTools/GeometryTypes.h \
    include/TetraMeshTools/GMSHMeshLoader.h \
    include/TetraMeshTools/GMSHMeshWriter.h \
    include/TetraMeshTools/Octree.h \
    include/TetraMeshTools/OctreeNode.h \
    include/TetraMeshTools/SATriangleBoxIntersection.h \
    include/TetraMeshTools/TetgenLoader.h \
    include/TetraMeshTools/TetgenWriter.h \
    include/TetraMeshTools/TetrahedronTopology.h \
    include/TetraMeshTools/TetraToolsExports.h \
    include/TetraMeshTools/TriangleTopology.h \
    include/TetraMeshTools/TriMeshLoader.h \
    include/TetraMeshTools/TriMeshWriter.h \
    include/TetraMeshTools/Vec2f.h \
    include/TetraMeshTools/Vec3f.h \
    include/TetraMeshTools/Vec4f.h \
    trimesh2/include/Box.h \
    trimesh2/include/bsphere.h \
    trimesh2/include/Color.h \
    trimesh2/include/ICP.h \
    trimesh2/include/KDtree.h \
    trimesh2/include/lineqn.h \
    trimesh2/include/mempool.h \
    trimesh2/include/noise3d.h \
    trimesh2/include/strutil.h \
    trimesh2/include/timestamp.h \
    trimesh2/include/TriMesh.h \
    trimesh2/include/TriMesh_algo.h \
    trimesh2/include/Vec.h \
    trimesh2/include/XForm.h

SOURCES += \
    src/BaseMeshLoader.cpp \
    src/GMSHMeshLoader.cpp \
    src/GMSHMeshWriter.cpp \
    src/Octree.cpp \
    src/OctreeNode.cpp \
    src/SATriangleBoxIntersection.cpp \
    src/TetgenWriter.cpp \
    src/TetrahedronTopology.cpp \
    src/TriangleTopology.cpp \
    src/TriMeshLoader.cpp \
    src/TriMeshWriter.cpp \
    trimesh2/src/conn_comps.cc \
    trimesh2/src/diffuse.cc \
    trimesh2/src/edgeflip.cc \
    trimesh2/src/faceflip.cc \
    trimesh2/src/filter.cc \
    trimesh2/src/ICP.cc \
    trimesh2/src/KDtree.cc \
    trimesh2/src/lmsmooth.cc \
    trimesh2/src/overlap.cc \
    trimesh2/src/remove.cc \
    trimesh2/src/reorder_verts.cc \
    trimesh2/src/shared.cc \
    trimesh2/src/subdiv.cc \
    trimesh2/src/TriMesh_bounding.cc \
    trimesh2/src/TriMesh_connectivity.cc \
    trimesh2/src/TriMesh_curvature.cc \
    trimesh2/src/TriMesh_grid.cc \
    trimesh2/src/TriMesh_io.cc \
    trimesh2/src/TriMesh_normals.cc \
    trimesh2/src/TriMesh_pointareas.cc \
    trimesh2/src/TriMesh_stats.cc \
    trimesh2/src/TriMesh_tstrips.cc
