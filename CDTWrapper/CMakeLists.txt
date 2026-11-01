cmake_minimum_required(VERSION 3.16)
project(CDTWrapper VERSION 0.1)

# Faster exact predicates, but the binary then needs a CPU with AVX2 and FMA
option(QTM_CDT_AVX2 "Build the CDT module with AVX2/FMA" OFF)

# Header-only dependencies of CDT, pinned to tested commits
include(FetchContent)
FetchContent_Declare(nfg
  GIT_REPOSITORY https://github.com/MarcoAttene/nfg.git
  GIT_TAG aeec97547a67dc6904ab84b39b15d20c6d62f65a
  SOURCE_SUBDIR do-not-configure
)
FetchContent_Declare(indirect_predicates
  GIT_REPOSITORY https://github.com/MarcoAttene/Indirect_Predicates.git
  GIT_TAG f8f243d18a6851fe3128b7e09af5b3574259d700
  SOURCE_SUBDIR do-not-configure
)
FetchContent_Declare(delaunay3d
  GIT_REPOSITORY https://github.com/MarcoAttene/Delaunay3D.git
  GIT_TAG 362502e63abb7adb3bf5a7a3b1e1a303640f8812
  SOURCE_SUBDIR do-not-configure
)
FetchContent_MakeAvailable(nfg indirect_predicates delaunay3d)

add_library(CDTWrapper SHARED src/CDTWrapper.cpp include/CDTWrapper.h)
target_compile_features(CDTWrapper PRIVATE cxx_std_20)
target_include_directories(CDTWrapper
  PRIVATE
    include
    ${nfg_SOURCE_DIR}/include
    ${indirect_predicates_SOURCE_DIR}/include
    ${delaunay3d_SOURCE_DIR}/include
)
# errors in CDT throw instead of ending the process
target_compile_definitions(CDTWrapper PRIVATE NFG_IP_ERROR_THROWS)
target_link_libraries(CDTWrapper PRIVATE TetraMeshTools::Headers)

# The exact predicates need IEEE 754 compliant arithmetic without FMA contraction
if(MSVC)
  target_compile_options(CDTWrapper PRIVATE /fp:strict /GS- /D_CRT_SECURE_NO_WARNINGS)
  if(QTM_CDT_AVX2)
    target_compile_options(CDTWrapper PRIVATE /arch:AVX2)
  endif()
else()
  target_compile_options(CDTWrapper PRIVATE -frounding-math -ffp-contract=off
                                            -Wno-unknown-pragmas)
  if(QTM_CDT_AVX2)
    target_compile_options(CDTWrapper PRIVATE -mavx2 -mfma)
  endif()
endif()

add_custom_command(
  TARGET CDTWrapper
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy
    "$<TARGET_FILE:CDTWrapper>"
    "${CMAKE_BINARY_DIR}"
)
