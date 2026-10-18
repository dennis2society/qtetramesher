cmake_minimum_required(VERSION 3.16)
project(GMSHWrapper VERSION 0.1)

if(NOT DEFINED GMSH_INSTALL_PATH OR GMSH_INSTALL_PATH STREQUAL "")
  if(WIN32)
    set(GMSH_INSTALL_PATH "D:/devel/gmsh-4.15.2-Windows64-sdk" CACHE PATH
        "GMSH installation or SDK root")
  else()
    set(GMSH_INSTALL_PATH "/opt/gmsh_git/build/install" CACHE PATH
        "GMSH installation or SDK root")
  endif()
else()
  set(GMSH_INSTALL_PATH "${GMSH_INSTALL_PATH}" CACHE PATH
      "GMSH installation or SDK root")
endif()

add_library(GMSHWrapper SHARED src/GMSHWrapper.cpp)
target_include_directories(GMSHWrapper
  PRIVATE
    include
    ${GMSH_INSTALL_PATH}/include
)

find_library(GMSH_LIBRARY
  NAMES gmsh gmsh-4.15.2
  PATHS
    ${GMSH_INSTALL_PATH}/lib
    ${GMSH_INSTALL_PATH}
  NO_DEFAULT_PATH
  REQUIRED
)

target_link_libraries(GMSHWrapper
  PRIVATE
    TetraMeshTools::Headers
    ${GMSH_LIBRARY}
)

if(WIN32)
  # The DLL name is versioned (gmsh-4.15.dll) and its folder differs between SDKs.
  file(GLOB GMSH_DLLS
    "${GMSH_INSTALL_PATH}/bin/gmsh*.dll"
    "${GMSH_INSTALL_PATH}/lib/gmsh*.dll"
    "${GMSH_INSTALL_PATH}/gmsh*.dll"
  )
  if(GMSH_DLLS)
    add_custom_command(TARGET GMSHWrapper POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${GMSH_DLLS}
        "$<TARGET_FILE_DIR:GMSHWrapper>"
    )
  else()
    message(WARNING "GMSH DLL not found in ${GMSH_INSTALL_PATH}, copy it next "
                    "to QTetraMesher.exe.")
  endif()
endif()

add_custom_command(
  TARGET GMSHWrapper
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
    "$<TARGET_FILE:GMSHWrapper>"
    "${CMAKE_BINARY_DIR}"
)
