# The optional meshop toolkit: its manifest lets the engine discover this folder; it is a
# separate target because it depends on VCGlib (GPL-3.0). See include/paperboat/toolkit/meshop.hpp.

include(FetchContent)

# VCGlib is header-only: find a checkout, else fetch a pinned release and use its root as the
# include directory. SOURCE_SUBDIR points at a folder with no CMakeLists.txt, so VCGlib's own
# build (examples, apps) never runs.
set(VCGLIB_TAG "2025.07")
find_path(VCGLIB_INCLUDE_DIR vcg/complex/complex.h)
option(PAPERBOAT_WITH_MESHOP "Build the meshop toolkit package (paperboat::toolkit_meshop)" ON)
if(NOT PAPERBOAT_WITH_MESHOP)
  return()
endif()
if(NOT VCGLIB_INCLUDE_DIR)
  FetchContent_Declare(vcglib
    URL https://github.com/cnr-isti-vclab/vcglib/archive/refs/tags/${VCGLIB_TAG}.tar.gz
    URL_HASH SHA256=e49fc9342d5476b3e39a5e1939b965b57c91d7a17b4f97b8c5eaf01228b16cf0
    DOWNLOAD_EXTRACT_TIMESTAMP ON
    SOURCE_SUBDIR "cmake/do-not-build")
  FetchContent_MakeAvailable(vcglib)
  set(VCGLIB_INCLUDE_DIR "${vcglib_SOURCE_DIR}")
endif()
message(STATUS "meshop: VCGlib at ${VCGLIB_INCLUDE_DIR}")

# VCGlib's math layer includes Eigen (header-only, MPL-2.0): use an installed copy, else fetch.
find_package(Eigen3 3.3 QUIET NO_MODULE)
if(NOT Eigen3_FOUND)
  FetchContent_Declare(eigen
    URL https://gitlab.com/libeigen/eigen/-/archive/5.0.1/eigen-5.0.1.tar.gz
    URL_HASH SHA256=e9c326dc8c05cd1e044c71f30f1b2e34a6161a3b6ecf445d56b53ff1669e3dec
    DOWNLOAD_EXTRACT_TIMESTAMP ON
    SOURCE_SUBDIR "cmake/do-not-build")
  FetchContent_MakeAvailable(eigen)
  add_library(Eigen3::Eigen INTERFACE IMPORTED)
  set_target_properties(Eigen3::Eigen PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${eigen_SOURCE_DIR}")
endif()

# VCGlib is header-only except for its PLY reader/writer (wrap/ply/plylib.cpp).
add_library(paperboat_toolkit_meshop
  src/mesh.cpp
  src/blocks.cpp
  "${VCGLIB_INCLUDE_DIR}/wrap/ply/plylib.cpp")
add_library(paperboat::toolkit_meshop ALIAS paperboat_toolkit_meshop)
target_include_directories(paperboat_toolkit_meshop PUBLIC include)
# VCGlib is a system header tree: its warnings are not ours to fix.
target_include_directories(paperboat_toolkit_meshop SYSTEM PRIVATE "${VCGLIB_INCLUDE_DIR}")
# The engine is C++20. In-tree this folder inherits the engine's standard; standalone the target
# is defined in this project, so it has to ask for C++20 itself.
target_compile_features(paperboat_toolkit_meshop PUBLIC cxx_std_20)
target_link_libraries(paperboat_toolkit_meshop
  PUBLIC paperboat::toolkit
  PRIVATE paperboat::warnings Eigen3::Eigen)

# The package's own test: a unit cube round-trips through every format and the checks agree.
if(BUILD_TESTING)
  add_executable(meshop_smoke tests/smoke.cpp)
  target_link_libraries(meshop_smoke PRIVATE paperboat::toolkit paperboat::toolkit_meshop)
  add_test(NAME meshop_smoke COMMAND meshop_smoke)
endif()
