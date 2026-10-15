cmake_minimum_required(VERSION 3.16)
project(GMSHWrapper VERSION 0.1)

# Set GMSH installation path
set(GMSH_INSTALL_PATH "/opt/gmsh_git/build/install")

include_directories(
  ${GMSH_INSTALL_PATH}/include
  PRIVATE
  include
)

# Find GMSH library generically (handles both static and shared libraries)
find_library(GMSH_LIBRARY
  NAMES gmsh
  PATHS ${GMSH_INSTALL_PATH}/lib
  NO_DEFAULT_PATH
  REQUIRED
)

add_library(
  GMSHWrapper
  SHARED
  src/GMSHWrapper.cpp
)

# Link against the GMSH library (platform-independent)
# CMake will automatically use .a on Linux, .dll on Windows, etc.
target_link_libraries(
  GMSHWrapper
  PRIVATE
  TetraMeshTools::Headers
  ${GMSH_LIBRARY}
)

add_custom_command(
  TARGET GMSHWrapper
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
    "$<TARGET_FILE:GMSHWrapper>"
    "${CMAKE_BINARY_DIR}"
)
