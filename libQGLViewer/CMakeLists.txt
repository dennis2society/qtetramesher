cmake_minimum_required(VERSION 3.16)
project(QGLViewer VERSION 2.6.3)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
# Instruct CMake to run uic automatically when needed.
set(CMAKE_AUTOUIC ON)

if (QT_MAJOR_VERSION EQUAL 6)
  find_package(Qt${QT_MAJOR_VERSION} REQUIRED COMPONENTS Core Widgets Xml OpenGLWidgets OpenGL)
  set(QtLibs Qt::Core Qt::Widgets Qt::Xml Qt::OpenGL Qt::OpenGLWidgets)
else ()
  find_package(Qt${QT_MAJOR_VERSION} REQUIRED COMPONENTS Core Widgets Xml OpenGL)
  set(QtLibs Qt::Core Qt::Widgets Qt::Xml Qt::OpenGL)
endif ()

find_package(OpenGL REQUIRED)

include_directories(
  "${PROJECT_SOURCE_DIR}/QGLViewer/"
)

# QGLViewer target.
set(QGLViewer_SRC
    "${PROJECT_SOURCE_DIR}/QGLViewer/VRender/BackFaceCullingOptimizer.cpp"
    "${PROJECT_SOURCE_DIR}/QGLViewer/VRender/BSPSortMethod.cpp"
    "${PROJECT_SOURCE_DIR}/QGLViewer/VRender/EPSExporter.cpp"
    "${PROJECT_SOURCE_DIR}/QGLViewer/VRender/Exporter.cpp"
    "${PROJECT_SOURCE_DIR}/QGLViewer/VRender/FIGExporter.cpp"
    "${PROJECT_SOURCE_DIR}/QGLViewer/VRender/gpc.cpp"
    "${PROJECT_SOURCE_DIR}/QGLViewer/VRender/NVector3.cpp"
    "${PROJECT_SOURCE_DIR}/QGLViewer/VRender/ParserGL.cpp"
    "${PROJECT_SOURCE_DIR}/QGLViewer/VRender/Primitive.cpp"
    "${PROJECT_SOURCE_DIR}/QGLViewer/VRender/PrimitivePositioning.cpp"
    "${PROJECT_SOURCE_DIR}/QGLViewer/VRender/TopologicalSortMethod.cpp"
    "${PROJECT_SOURCE_DIR}/QGLViewer/VRender/Vector2.cpp"
    "${PROJECT_SOURCE_DIR}/QGLViewer/VRender/Vector3.cpp"
    "${PROJECT_SOURCE_DIR}/QGLViewer/VRender/VisibilityOptimizer.cpp"
    "${PROJECT_SOURCE_DIR}/QGLViewer/VRender/VRender.cpp"
    "${PROJECT_SOURCE_DIR}/QGLViewer/camera.cpp"
    "${PROJECT_SOURCE_DIR}/QGLViewer/constraint.cpp"
    "${PROJECT_SOURCE_DIR}/QGLViewer/frame.cpp"
    "${PROJECT_SOURCE_DIR}/QGLViewer/keyFrameInterpolator.cpp"
    "${PROJECT_SOURCE_DIR}/QGLViewer/manipulatedCameraFrame.cpp"
    "${PROJECT_SOURCE_DIR}/QGLViewer/manipulatedFrame.cpp"
    "${PROJECT_SOURCE_DIR}/QGLViewer/mouseGrabber.cpp"
    "${PROJECT_SOURCE_DIR}/QGLViewer/qglviewer.cpp"
    "${PROJECT_SOURCE_DIR}/QGLViewer/quaternion.cpp"
    "${PROJECT_SOURCE_DIR}/QGLViewer/saveSnapshot.cpp"
    "${PROJECT_SOURCE_DIR}/QGLViewer/vec.cpp")
add_library(QGLViewer SHARED ${QGLViewer_SRC})
target_include_directories(QGLViewer INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(QGLViewer PRIVATE ${QtLibs} OpenGL::GL OpenGL::GLU)

if(WIN32)
  add_compile_definitions(NOMINMAX)
  target_compile_options(${PROJECT_NAME} PRIVATE "-DCREATE_QGLVIEWER_DLL")
  # remove warnings about deprecation (CRT,etc)
  target_compile_options(${PROJECT_NAME} PRIVATE "/wd4996")
endif()

#add_compile_options(-O3 -Wall -Wextra -Wpedantic -fopenmp)

add_custom_command(
  TARGET QGLViewer
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
    "$<TARGET_FILE:QGLViewer>"
    "${CMAKE_BINARY_DIR}"
)
