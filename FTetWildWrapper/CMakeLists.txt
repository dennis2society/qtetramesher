cmake_minimum_required(VERSION 3.20)
project(FTetWildWrapper VERSION 0.1)

# Optional module: when a requirement is missing, it is skipped with a warning
# and QTetraMesher is built without fTetWild.
if(WIN32)
  set(FTETWILD_DEFAULT_PATH "D:/devel/ftetwild_git")
else()
  set(FTETWILD_DEFAULT_PATH "/opt/ftetwild_git")
endif()
set(FTETWILD_SOURCE_DIR "${FTETWILD_DEFAULT_PATH}" CACHE PATH
    "fTetWild source directory (git repository)")

if(NOT EXISTS "${FTETWILD_SOURCE_DIR}/CMakeLists.txt")
  message(WARNING "fTetWild not found in FTETWILD_SOURCE_DIR "
                  "('${FTETWILD_SOURCE_DIR}'), building without fTetWild.")
  return()
endif()

# fTetWild stops the configuration with an error if GMP is missing
# On Windows GMP comes with CGAL (<CGAL>/auxiliary/gmp)
if(WIN32 AND CGAL_DIR)
  get_filename_component(FTETWILD_CGAL_ROOT "${CGAL_DIR}/../../.." ABSOLUTE)
  set(FTETWILD_GMP_HINT "${FTETWILD_CGAL_ROOT}/auxiliary/gmp")
endif()
find_path(GMP_INCLUDE_DIRS NAMES gmp.h
          HINTS "${GMP_INCLUDE_DIR}" "${FTETWILD_GMP_HINT}/include"
          PATHS $ENV{GMP_INC})
find_library(GMP_LIBRARIES NAMES gmp libgmp mpir
             HINTS "${FTETWILD_GMP_HINT}/lib"
             PATHS $ENV{GMP_LIB})
if(NOT GMP_INCLUDE_DIRS OR NOT GMP_LIBRARIES)
  message(WARNING "GMP (required by fTetWild) not found, building without "
                  "fTetWild. Set GMP_INCLUDE_DIRS and GMP_LIBRARIES.")
  return()
endif()

# Provide Eigen3::Eigen before libigl creates it: libigl's own Eigen download
# uses FetchContent_Populate(), which CMake 4 rejects.
# (the package registry may point to Eigen build trees of other projects)
find_package(Eigen3 3.3 QUIET NO_MODULE NO_CMAKE_PACKAGE_REGISTRY)
if(NOT TARGET Eigen3::Eigen)
  include(FetchContent)
  FetchContent_Declare(
    eigen
    GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
    GIT_TAG 3.4.0
    GIT_SHALLOW TRUE
    SOURCE_SUBDIR do_not_add_subdirectory
  )
  FetchContent_MakeAvailable(eigen)
  add_library(Eigen3_Eigen INTERFACE)
  target_include_directories(Eigen3_Eigen SYSTEM INTERFACE ${eigen_SOURCE_DIR})
  add_library(Eigen3::Eigen ALIAS Eigen3_Eigen)
endif()

# Patch fTetWild so that it writes no __tracked_surface.stl debug file into
# the working directory when used as a library (without an output path).
set(FTETWILD_PATCH_FILE "${FTETWILD_SOURCE_DIR}/src/MeshImprovement.cpp")
set(FTETWILD_PATCH_OLD
    "igl::writeSTL(mesh.params.output_path + \"_\" + mesh.params.postfix + \"_tracked_surface.stl\", V_sf, F_sf);")
set(FTETWILD_PATCH_NEW
    "if (!mesh.params.output_path.empty()) ${FTETWILD_PATCH_OLD}")
file(READ "${FTETWILD_PATCH_FILE}" FTETWILD_PATCH_CONTENT)
string(FIND "${FTETWILD_PATCH_CONTENT}" "${FTETWILD_PATCH_NEW}"
       FTETWILD_PATCH_APPLIED)
string(FIND "${FTETWILD_PATCH_CONTENT}" "${FTETWILD_PATCH_OLD}"
       FTETWILD_PATCH_FOUND)
if(FTETWILD_PATCH_APPLIED EQUAL -1)
  if(FTETWILD_PATCH_FOUND EQUAL -1)
    message(WARNING "Could not patch ${FTETWILD_PATCH_FILE}, fTetWild will "
                    "write __tracked_surface.stl files.")
  else()
    string(REPLACE "${FTETWILD_PATCH_OLD}" "${FTETWILD_PATCH_NEW}"
           FTETWILD_PATCH_CONTENT "${FTETWILD_PATCH_CONTENT}")
    file(WRITE "${FTETWILD_PATCH_FILE}" "${FTETWILD_PATCH_CONTENT}")
    message(STATUS "Patched ${FTETWILD_PATCH_FILE}")
  endif()
endif()

add_subdirectory(${FTETWILD_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/fTetWild
                 EXCLUDE_FROM_ALL)

add_library(
  FTetWildWrapper
  SHARED
  src/FTetWildWrapper.cpp
)

target_include_directories(FTetWildWrapper PUBLIC include)

target_link_libraries(
  FTetWildWrapper
  PUBLIC
  TetraMeshTools::Headers
  PRIVATE
  TetraMeshTools
  FloatTetwild
)

target_compile_features(FTetWildWrapper PRIVATE cxx_std_17)

add_custom_command(
  TARGET FTetWildWrapper
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
    "$<TARGET_FILE:FTetWildWrapper>"
    "${CMAKE_BINARY_DIR}"
)

# Copy the shared runtime dependencies next to QTetraMesher. Everything else
# (libigl, spdlog, fmt, ...) is linked statically; the shared geogram is
# copied by TetraMeshTools.
if(TARGET TBB::tbb)
  get_target_property(FTETWILD_TBB_TARGET TBB::tbb ALIASED_TARGET)
  if(NOT FTETWILD_TBB_TARGET)
    set(FTETWILD_TBB_TARGET TBB::tbb)
  endif()
  get_target_property(FTETWILD_TBB_TYPE ${FTETWILD_TBB_TARGET} TYPE)
  if(FTETWILD_TBB_TYPE STREQUAL "SHARED_LIBRARY")
    if(WIN32)
      set(FTETWILD_TBB_FILES "$<TARGET_FILE:${FTETWILD_TBB_TARGET}>")
    else()
      # libtbb.so.<full version> and libtbb.so.<major>, which is loaded
      set(FTETWILD_TBB_FILES "$<TARGET_FILE:${FTETWILD_TBB_TARGET}>"
                             "$<TARGET_SONAME_FILE:${FTETWILD_TBB_TARGET}>")
    endif()
    add_custom_command(
      TARGET FTetWildWrapper
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy ${FTETWILD_TBB_FILES}
        "${CMAKE_BINARY_DIR}"
    )
  endif()
endif()

# On Windows GMP/MPIR is a DLL next to its import library or in ../bin.
# On Linux the system GMP is used.
if(WIN32)
  get_filename_component(FTETWILD_GMP_LIB_DIR "${GMP_LIBRARIES}" DIRECTORY)
  find_file(GMP_DLL
    NAMES gmp.dll libgmp-10.dll gmp-10.dll mpir.dll
    HINTS "${FTETWILD_GMP_LIB_DIR}" "${FTETWILD_GMP_LIB_DIR}/../bin"
         "${FTETWILD_GMP_HINT}/bin"
    NO_DEFAULT_PATH
    DOC "GMP/MPIR DLL copied next to QTetraMesher"
  )
  if(GMP_DLL)
    add_custom_command(
      TARGET FTetWildWrapper
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy "${GMP_DLL}" "${CMAKE_BINARY_DIR}"
    )
  else()
    message(WARNING "GMP DLL not found, copy it next to QTetraMesher.exe or "
                    "set GMP_DLL.")
  endif()
endif()
