cmake_minimum_required(VERSION 3.16)
project(QTetraMesher VERSION 0.8.6)
# Optional identifier after the version, e.g. "_RC1" for 0.8.5_RC1 (project()
# only accepts numbers). Used in the app and the package names.
set(QTM_VERSION_SUFFIX "")

if(POLICY CMP0167)
  cmake_policy(SET CMP0167 OLD)
endif()

# Write compile_commands.json to the build folder (used by the IDE, e.g. the
# VSCode C/C++ extension or clangd, to find the include paths)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# This line is required to use the PROJECT_VERSION set above
# in the code.
# QTM_VERSION_OVERRIDE replaces the version shown in the app, e.g. 0.8.6-rc1
set(QTM_VERSION_OVERRIDE "" CACHE STRING "Version shown in the app (empty: project version and suffix)")
if(QTM_VERSION_OVERRIDE)
  set(QTM_VERSION_STRING "${QTM_VERSION_OVERRIDE}")
else()
  set(QTM_VERSION_STRING "${PROJECT_VERSION}${QTM_VERSION_SUFFIX}")
endif()
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

# geogram as one shared library for TetraMeshTools (surface repair) and
# fTetWild, which skips its own static geogram when geogram::geogram exists.
include(FetchContent)
FetchContent_Declare(
  geogram
  GIT_REPOSITORY https://github.com/BrunoLevy/geogram
  GIT_TAG v1.9.6
)
if(MSVC)
  set(VORPALINE_PLATFORM "Win-vs-generic")
  set(VORPALINE_ARCH_64 TRUE CACHE BOOL "" FORCE)
elseif(APPLE)
  set(VORPALINE_PLATFORM "Darwin-clang")
else()
  set(VORPALINE_PLATFORM "Linux64-gcc")
endif()
set(VORPALINE_BUILD_DYNAMIC TRUE CACHE BOOL "Build geogram as shared library" FORCE)
set(GEOGRAM_SUB_BUILD ON CACHE BOOL "" FORCE)
set(GEOGRAM_LIB_ONLY ON CACHE BOOL "" FORCE)
set(GEOGRAM_WITH_GRAPHICS OFF CACHE BOOL "" FORCE)
set(GEOGRAM_WITH_LUA OFF CACHE BOOL "" FORCE)
set(GEOGRAM_WITH_EXPLORAGRAM OFF CACHE BOOL "" FORCE)
set(GEOGRAM_WITH_LEGACY_NUMERICS OFF CACHE BOOL "" FORCE)
set(GEOGRAM_WITH_TRIANGLE OFF CACHE BOOL "" FORCE)
set(GEOGRAM_WITH_TETGEN OFF CACHE BOOL "" FORCE)
# optimizer for the Newton iterations of the remeshing
set(GEOGRAM_WITH_HLBFGS ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(geogram)
add_library(qtm_geogram INTERFACE)
add_library(geogram::geogram ALIAS qtm_geogram)
target_link_libraries(qtm_geogram INTERFACE geogram)
# dllimport on Windows
target_compile_definitions(qtm_geogram INTERFACE GEO_DYNAMIC_LIBS)
target_include_directories(qtm_geogram SYSTEM INTERFACE
                           "${geogram_SOURCE_DIR}/src/lib")
if(NOT MSVC)
  find_package(OpenMP REQUIRED)
  target_link_libraries(qtm_geogram INTERFACE OpenMP::OpenMP_CXX)
endif()

add_subdirectory(QtmGLViewer)
add_subdirectory(TetraMeshTools)
add_subdirectory(SofaTetraStuffing)
add_subdirectory(CGALTetrahedralize)
add_subdirectory(TetgenWrapper)
add_subdirectory(GMSHWrapper)
add_subdirectory(GenericIsosurfaceStuffing)

# fTetWild module, skipped if fTetWild or one of its requirements is missing.
option(QTM_WITH_FTETWILD "Build the fTetWild module" ON)
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
  src/SurfaceTransformWidget.hpp
  src/SurfaceTransformWidget.cpp
  src/SurfaceRepairWidget.hpp
  src/SurfaceRepairWidget.cpp
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
