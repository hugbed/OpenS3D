cmake_minimum_required(VERSION 3.2)

# You can tweak some common (for all subprojects) stuff here. For example:

set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_DISABLE_SOURCE_CHANGES  ON)

if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(SEND_ERROR "In-source builds are not allowed.")
endif ()

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE   ON)

# Remove 'lib' prefix for shared libraries on Windows
if (WIN32)
  set(CMAKE_SHARED_LIBRARY_PREFIX "")
endif ()

# cmake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

# clang-tidy and clang-format
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include(cmake/clang-cxx-dev-tools.cmake)

# GTest
set(ENV{CTEST_OUTPUT_ON_FAILURE} true)
set(ENV{GTEST_COLOR} true)
enable_testing()

# When done tweaking common stuff, configure the components (subprojects).
# NOTE: The order matters! The most independent ones should go first.
add_subdirectory(libs/googletest)
add_subdirectory(libs/gsl)
add_subdirectory(libs/decklink)
add_subdirectory(core/s3d)
#add_subdirectory(core/s3dcuda)
add_subdirectory(apps/OgreS3DPlayer)
add_subdirectory(apps/S3DDemo)
add_subdirectory(apps/S3DCudaDemo)
