cmake_minimum_required(VERSION 3.16)
project(GMSHWrapper VERSION 0.1)

include_directories(
  ../TetraMeshTools/include
  ../TetraMeshTools/include/TetraMeshTools
  PRIVATE
  include
)

add_library(
  GMSHWrapper
  SHARED
  include/GMSHWrapper.h
  src/GMSHWrapper.cpp
)

target_link_libraries(
  GMSHWrapper
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/libgmsh.so
)

add_custom_command(
  TARGET GMSHWrapper
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
    "$<TARGET_FILE:GMSHWrapper>"
    "${CMAKE_BINARY_DIR}"
)
