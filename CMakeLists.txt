cmake_minimum_required(VERSION 3.16)
project(QTetraMesher VERSION 0.8.5)

if(POLICY CMP0167)
  cmake_policy(SET CMP0167 OLD)
endif()

# Write compile_commands.json to the build folder (used by the IDE, e.g. the
# VSCode C/C++ extension or clangd, to find the include paths)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# This line is required to use the PROJECT_VERSION set above
# in the code.
configure_file("src/qtetramesher_version.h.in" "qtetramesher_version.h")

if(WIN32)
  set(SOFA_DEFAULT_PATH "D:/devel/sofa_git/build/install")
else()
  set(SOFA_DEFAULT_PATH "/opt/sofa_git/build/install")
endif()
set(SOFA_INSTALL_DIR "${SOFA_DEFAULT_PATH}" CACHE PATH "Sofa installation directory")

list(APPEND CMAKE_PREFIX_PATH ${SOFA_INSTALL_DIR}/lib/cmake)

find_package(Sofa.Config REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets WidgetsTools Xml OpenGLWidgets OpenGL Concurrent)
set(QtLibs Qt::Core Qt::Widgets Qt::Xml Qt::OpenGL Qt::OpenGLWidgets)
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

# MSVC only defines M_PI when this is set before <cmath>
if(MSVC)
  add_compile_definitions(_USE_MATH_DEFINES NOMINMAX)
endif()

add_subdirectory(QtmGLViewer)
add_subdirectory(TetraMeshTools)
add_subdirectory(SofaTetraStuffing)
add_subdirectory(CGALTetrahedralize)
add_subdirectory(TetgenWrapper)
add_subdirectory(GMSHWrapper)
add_subdirectory(GenericIsosurfaceStuffing)

# Optional fTetWild module (off by default on Windows, where it is untested).
# If fTetWild or one of its requirements is missing, the module is skipped.
if(WIN32)
  set(QTM_WITH_FTETWILD_DEFAULT OFF)
else()
  set(QTM_WITH_FTETWILD_DEFAULT ON)
endif()
option(QTM_WITH_FTETWILD "Build the optional fTetWild module"
       ${QTM_WITH_FTETWILD_DEFAULT})
if(QTM_WITH_FTETWILD)
  if(CMAKE_VERSION VERSION_LESS 3.20)
    message(WARNING "fTetWild needs CMake 3.20, building without fTetWild.")
  else()
    add_subdirectory(FTetWildWrapper)
  endif()
endif()

set(CMAKE_INCLUDE_CURRENT_DIR "YES")
set(CMAKE_AUTOMOC "YES")
set(CMAKE_AUTOUIC "YES")
set(CMAKE_AUTORCC "YES")

include_directories(
  src
  SofaTetraStuffing/include
  CGALTetrahedralize/include
  GenericIsosurfaceStuffing/include
  TetgenWrapper/include
  GMSHWrapper/include
  ${Qt6Core_INCLUDE_DIRS}
  ${Qt6Widgets_INCLUDE_DIRS}
  ${Qt6Xml_INCLUDE_DIRS}
  ${Qt6OpenGL_INCLUDE_DIRS}
)

add_executable(
  QTetraMesher
  src/BaseOptionsWidget.hpp
  src/BaseOptionsWidget.cpp
  src/SurfaceVisWidget.hpp
  src/SurfaceVisWidget.cpp
  src/TetraVisWidget.hpp
  src/TetraVisWidget.cpp
  src/OctreeVisWidget.hpp
  src/OctreeVisWidget.cpp
  src/main.cpp
  src/QGLTetraMesh.hpp
  src/mainwindow.hpp
  src/mainwindow.cpp
  src/qgltetraviewer.cpp
  src/Timer.cpp
  src/qgltetraviewer.hpp
  src/Timer.h
  src/QGLTetraMesh.cpp
  src/SofaTetraStuffingWidget.hpp
  src/SofaTetraStuffingWidget.cpp
  src/CGALTetrahedralizeWidget.hpp
  src/CGALTetrahedralizeWidget.cpp
  src/TetgenWidget.hpp
  src/TetgenWidget.cpp
  src/GMSHWidget.hpp
  src/GMSHWidget.cpp
  src/FTetWildWidget.hpp
  src/FTetWildWidget.cpp
  src/GenericIsosurfaceStuffingWidget.hpp
  src/GenericIsosurfaceStuffingWidget.cpp
  src/RangeSlider.hpp
  src/RangeSlider.cpp
  src/TetraJob.hpp
  src/TetraJob.cpp
  src/TetraMethodWidget.hpp
  resources/qtetramesher.qrc
  FTetWildWrapper/include/FTetWildWrapper.h
)

# embeds the application icon into the Windows executable
if (WIN32)
  target_sources(QTetraMesher PRIVATE resources/qtetramesher.rc)
endif()

target_link_libraries(
  QTetraMesher
  TetraMeshTools
  SofaTetraStuffing
  CGALTetrahedralize
  GenericIsosurfaceStuffing
  TetgenWrapper
  GMSHWrapper
  CGAL
  ${SofaCore}
  ${SofaComponentEngineGenerator}
  Qt6::Core
  Qt6::Widgets
  Qt6::OpenGL
  Qt6::Xml
  Qt6::Concurrent
  QtmGLViewer
  Qt6::OpenGLWidgets
)

# Sofa namespaces require C++17 features
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

if(TARGET FTetWildWrapper)
  message(STATUS "fTetWild module enabled")
  target_link_libraries(QTetraMesher FTetWildWrapper)
  target_compile_definitions(QTetraMesher PRIVATE QTM_HAVE_FTETWILD)
endif()

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
