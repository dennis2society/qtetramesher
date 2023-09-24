cmake_minimum_required(VERSION 3.16)
project(QTetraMesher VERSION 0.8.3)

# Set this to where you have installed your Sofa build:
set(SOFA_INSTALL_DIR "/opt/sofa_git/build/install/" CACHE PATH "")

list(APPEND CMAKE_PREFIX_PATH ${SOFA_INSTALL_DIR}/lib/cmake)


find_package(Sofa.Config REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Core Widgets Xml OpenGL)
find_package(GLUT REQUIRED)
find_package(OpenGL REQUIRED COMPONENTS OpenGL)
find_package(QGLViewer REQUIRED)
find_package(CGAL REQUIRED)

find_library(SofaCore
             NAMES Sofa.Core
             HINTS ${SOFA_INSTALL_DIR}/lib
)

find_library(SofaComponentEngineGenerator
             NAMES Sofa.Component.Engine.Generate
             HINTS ${SOFA_INSTALL_DIR}/lib
)

if(NOT SofaCore)
  message(FATAL_ERROR "SofaCore library not found")
endif()

add_subdirectory(TetraMeshTools)
add_subdirectory(SofaTetraStuffing)
add_subdirectory(CGALTetrahedralize)
add_subdirectory(quartet_tetrastuffing)

set(CMAKE_INCLUDE_CURRENT_DIR "YES")
set(CMAKE_AUTOMOC "YES")
set(CMAKE_AUTOUIC "YES")

include_directories(
  include
  src
  TetraMeshTools/include
  TetraMeshTools/include/TetraMeshTools
  SofaTetraStuffing/include
  CGALTetrahedralize/include
  quartet_tetrastuffing/include
  ${Qt5Core_INCLUDE_DIRS}
  ${Qt5Widgets_INCLUDE_DIRS}
  ${Qt5Xml_INCLUDE_DIRS}
  ${Qt5OpenGL_INCLUDE_DIRS}
)

add_executable(
  QTetraMesher
  src/BaseTetraDialog.cpp
  src/main.cpp
  src/QGLTetraMesh.h
  src/quartettetrahedralizedialog.cpp
  src/tetrastuffingdialog.h
  src/BaseTetraDialog.h
  src/mainwindow.cpp
  src/qgltetraviewer.cpp
  src/Timer.cpp
  src/cgaltetrahedralizedialog.cpp
  src/mainwindow.h
  src/qgltetraviewer.h
  src/Timer.h
  src/cgaltetrahedralizedialog.h
  src/QGLTetraMesh.cpp
  src/quartetterahedralizedialog.h
  src/tetrastuffingdialog.cpp
  src/mainwindow.ui
  src/tetrastuffingdialog.ui
  src/CGALTetrahedralizeDialog.ui
  src/quartettetrastuffingdialog.ui
)

target_link_libraries(
  QTetraMesher
  TetraMeshTools
  SofaTetraStuffing
  CGALTetrahedralize
  QuartetTetraStuffing
  CGAL
  ${SofaCore}
  ${SofaComponentEngineGenerator}
  Qt5::Core
  Qt5::Widgets
  Qt5::OpenGL
  Qt5::Xml
  ${GLUT_LIBRARY}
  OpenGL::OpenGL
  # for Ubuntu this line must be changed to QGLViewer-qt5
  QGLViewer
)

add_custom_command(
  TARGET QTetraMesher
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
    "$<TARGET_FILE:QTetraMesher>"
    "${CMAKE_BINARY_DIR}"
)
