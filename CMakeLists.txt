# paperboat-toolkit-meshop: mesh operations for Paperboat.
#
# Builds paperboat::toolkit_meshop on VCGlib. Two modes:
#   1. In-tree: copy engine/toolkit/meshop into a paperboat checkout at engine/toolkit/meshop/
#      and configure the engine normally (the folder is discovered from its toolkit.toml).
#   2. Standalone: set PAPERBOAT_SOURCE_DIR to a paperboat checkout, or let it fetch the engine.

cmake_minimum_required(VERSION 3.25)
project(paperboat_toolkit_meshop LANGUAGES CXX)

include(CTest)  # BUILD_TESTING and the toolkit's smoke test
include(FetchContent)

if(NOT DEFINED PAPERBOAT_SOURCE_DIR OR NOT EXISTS "${PAPERBOAT_SOURCE_DIR}/engine/CMakeLists.txt")
  message(STATUS "PAPERBOAT_SOURCE_DIR not set; fetching paperboat from GitHub")
  FetchContent_Declare(paperboat
    GIT_REPOSITORY https://github.com/paperboatlab/paperboat.git
    GIT_TAG dev)
  FetchContent_MakeAvailable(paperboat)
  set(PAPERBOAT_SOURCE_DIR "${paperboat_SOURCE_DIR}" CACHE PATH "Path to the paperboat source tree")
endif()

add_subdirectory("${PAPERBOAT_SOURCE_DIR}/engine" "${CMAKE_BINARY_DIR}/paperboat-engine" EXCLUDE_FROM_ALL)
add_subdirectory(engine/toolkit/meshop)
