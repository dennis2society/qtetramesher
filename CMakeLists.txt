cmake_minimum_required(VERSION 3.16)
project(QTetraMesher VERSION 0.8.3)

# This line is required to use the PROJECT_VERSION set above
# in the code.
configure_file("src/qtetramesher_version.h.in" "qtetramesher_version.h")

# Set this line to where you have installed your Sofa build:
set(SOFA_INSTALL_DIR "/opt/sofa_git/build/install" CACHE PATH "")

list(APPEND CMAKE_PREFIX_PATH ${SOFA_INSTALL_DIR}/lib/cmake)

find_package(Sofa.Config REQUIRED)
message(INFO "")
#find_package(Qt${QT_MAJOR_VERSION} REQUIRED COMPONENTS Core Widgets Xml OpenGL)
if (QT_MAJOR_VERSION EQUAL 6)
  find_package(Qt${QT_MAJOR_VERSION} REQUIRED COMPONENTS Core Widgets Xml OpenGLWidgets OpenGL)
  set(QtLibs Qt::Core Qt::Widgets Qt::Xml Qt::OpenGL Qt::OpenGLWidgets)
else ()
  find_package(Qt${QT_MAJOR_VERSION} REQUIRED COMPONENTS Core Widgets Xml OpenGL)
  set(QtLibs Qt::Core Qt::Widgets Qt::Xml Qt::OpenGL)
endif ()
find_package(GLUT REQUIRED)
find_package(OpenGL REQUIRED COMPONENTS OpenGL)
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

add_subdirectory(libQGLViewer)
add_subdirectory(TetraMeshTools)
add_subdirectory(SofaTetraStuffing)
add_subdirectory(CGALTetrahedralize)
add_subdirectory(TetgenWrapper)
add_subdirectory(quartet_tetrastuffing)

set(CMAKE_INCLUDE_CURRENT_DIR "YES")
set(CMAKE_AUTOMOC "YES")
set(CMAKE_AUTOUIC "YES")

include_directories(
  src
  TetraMeshTools/include
  TetraMeshTools/include/TetraMeshTools
  SofaTetraStuffing/include
  CGALTetrahedralize/include
  quartet_tetrastuffing/include
  TetgenWrapper/include
  libQGLViewer
  ${Qt${QT_MAJOR_VERSION}Core_INCLUDE_DIRS}
  ${Qt${QT_MAJOR_VERSION}Widgets_INCLUDE_DIRS}
  ${Qt${QT_MAJOR_VERSION}Xml_INCLUDE_DIRS}
  ${Qt${QT_MAJOR_VERSION}OpenGL_INCLUDE_DIRS}
)

set(UI_FILES
  src/mainwindow.ui
  src/tetrastuffingdialog.ui
  src/CGALTetrahedralizeDialog.ui
  src/quartettetrastuffingdialog.ui
  src/tetgendialog.ui
)

add_executable(
  QTetraMesher
  src/BaseOptionsWidget.hpp
  src/BaseOptionsWidget.cpp
  src/SurfaceVisWidget.hpp
  src/SurfaceVisWidget.cpp
  src/TetraVisWidget.hpp
  src/TetraVisWidget.cpp
  src/BaseTetraDialog.cpp
  src/OctreeVisWidget.hpp
  src/OctreeVisWidget.cpp
  src/main.cpp
  src/QGLTetraMesh.h
  src/quartettetrahedralizedialog.cpp
  src/tetrastuffingdialog.h
  src/BaseTetraDialog.h
  #src/mainwindow.cpp
  src/mainwindow2.cpp
  src/qgltetraviewer.cpp
  src/Timer.cpp
  src/cgaltetrahedralizedialog.cpp
  #src/mainwindow.h
  src/mainwindow2.hpp
  src/qgltetraviewer.h
  src/Timer.h
  src/cgaltetrahedralizedialog.h
  src/QGLTetraMesh.cpp
  src/quartetterahedralizedialog.h
  src/tetrastuffingdialog.cpp
  src/tetgendialog.h
  src/tetgendialog.cpp
  ${UI_FILES}
  src/SofaTetraStuffingWidget.hpp
  src/SofaTetraStuffingWidget.cpp
  src/CGALTetrahedralizeWidget.hpp
  src/CGALTetrahedralizeWidget.cpp
  src/QuartetTetraStuffingWidget.hpp
  src/QuartetTetraStuffingWidget.cpp
  src/TetgenWidget.hpp
  src/TetgenWidget.cpp
)

target_link_libraries(
  QTetraMesher
  TetraMeshTools
  SofaTetraStuffing
  CGALTetrahedralize
  QuartetTetraStuffing
  TetgenWrapper
  CGAL
  ${SofaCore}
  ${SofaComponentEngineGenerator}
  Qt${QT_MAJOR_VERSION}::Core
  Qt${QT_MAJOR_VERSION}::Widgets
  Qt${QT_MAJOR_VERSION}::OpenGL
  Qt${QT_MAJOR_VERSION}::Xml
  ${GLUT_LIBRARY}
  OpenGL::GL
  QGLViewer
)
if (${QT_MAJOR_VERSION} EQUAL 6)
  target_link_libraries(
    QTetraMesher
    Qt::OpenGLWidgets
  )
endif ()

# Sofa namespaces require C++17 features
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

# attempt to hide the console window on Windows
if (WIN32)
  set_target_properties(QTetraMesher PROPERTIES WIN32_EXECUTABLE TRUE) 
endif()

add_custom_command(
  TARGET QTetraMesher
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
    "$<TARGET_FILE:QTetraMesher>"
    "${CMAKE_BINARY_DIR}"
)
