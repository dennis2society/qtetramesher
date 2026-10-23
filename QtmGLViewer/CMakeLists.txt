cmake_minimum_required(VERSION 3.16)
project(QtmGLViewer VERSION 0.1 LANGUAGES CXX)

# Lightweight QOpenGLWidget based 3D viewer (OpenGL 3.3 core profile) that
# replaces libQGLViewer. Can be built standalone or via add_subdirectory().

option(QTMGLVIEWER_BUILD_TEST "Build the QtmGLViewer test application" ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets OpenGL OpenGLWidgets)

add_library(
  QtmGLViewer
  STATIC
  include/QtmGLViewer/Camera.hpp
  include/QtmGLViewer/MeshBuffer.hpp
  include/QtmGLViewer/MeshData.hpp
  include/QtmGLViewer/QtmGLViewer.hpp
  include/QtmGLViewer/RenderContext.hpp
  src/Camera.cpp
  src/MeshBuffer.cpp
  src/MeshData.cpp
  src/QtmGLViewer.cpp
  src/RenderContext.cpp
)
add_library(QtmGLViewer::QtmGLViewer ALIAS QtmGLViewer)

target_include_directories(QtmGLViewer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(
  QtmGLViewer
  PUBLIC
  Qt6::Core
  Qt6::Gui
  Qt6::Widgets
  Qt6::OpenGL
  Qt6::OpenGLWidgets
)

target_compile_features(QtmGLViewer PUBLIC cxx_std_17)

# AUTOMOC is set per target so the library does not depend on the parent
# project's CMAKE_AUTOMOC setting. PIC allows linking into shared libraries.
set_target_properties(
  QtmGLViewer
  PROPERTIES
  AUTOMOC ON
  POSITION_INDEPENDENT_CODE ON
)

if(QTMGLVIEWER_BUILD_TEST)
  add_executable(
    QtmGLViewerTest
    test/main.cpp
    test/TestMainWindow.hpp
    test/TestMainWindow.cpp
    test/MobiusViewer.hpp
    test/MobiusViewer.cpp
  )
  target_link_libraries(QtmGLViewerTest PRIVATE QtmGLViewer)
  set_target_properties(QtmGLViewerTest PROPERTIES AUTOMOC ON)
  if(WIN32)
    set_target_properties(QtmGLViewerTest PROPERTIES WIN32_EXECUTABLE TRUE)
  endif()
endif()
