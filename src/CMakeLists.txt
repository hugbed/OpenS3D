cmake_minimum_required(VERSION 3.2)

# You can tweak some common (for all subprojects) stuff here. For example:

set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_DISABLE_SOURCE_CHANGES  ON)

if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(SEND_ERROR "In-source builds are not allowed.")
endif ()

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE   ON)

# BUILD OPTIONS
option(OpenS3D_BUILD_APPS "Build OpenS3D applications" ON)
option(OpenS3D_BUILD_OGRE_APPS "Build Ogre applications" OFF)
option(OpenS3D_USE_CV "Build OpenS3D OpenCV implementations" ON)
option(OpenS3D_USE_CUDA "Build OpenS3D Cuda related code" OFF)
option(OpenS3D_USE_DECKLINK "Build OpenS3D DeckLink implementations" ON)
option(OpenS3D_USE_FFMPEG "Build OpenS3D FFmpeg implementations" ON)
option(OpenS3D_BUILD_COVERAGE "Enable code coverage generation (gcc only)" OFF)

# Remove 'lib' prefix for shared libraries on Windows
if (WIN32)
  set(CMAKE_SHARED_LIBRARY_PREFIX "")
endif ()

# cmake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

# enable code coverage generation (only with GCC)
if (OpenS3D_BUILD_COVERAGE AND CMAKE_COMPILER_IS_GNUCXX)
  message("OpenS3D_BUILD_COVERAGE enabled")
  set(CMAKE_BUILD_TYPE "Debug")
  add_definitions(--coverage -fprofile-arcs -ftest-coverage)
  set(CMAKE_EXE_LINKER_FLAGS
      "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage")
endif()

# clang-tidy and clang-format
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include(cmake/clang-cxx-dev-tools.cmake)

# Generate GTest solution
include(cmake/gtest-generator.txt)
enable_testing()

# eigen
set(EIGEN_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/libs/eigen)
include_directories(${EIGEN_INCLUDE_DIRS})

# Add project subdirectories
#add_subdirectory(libs/googletest)
add_subdirectory(libs/gsl)
add_subdirectory(core/s3d)

if (OpenS3D_USE_CV)
  add_subdirectory(core/cv)
endif()
if (OpenS3D_USE_DECKLINK)
  add_subdirectory(libs/decklink)
  add_subdirectory(core/decklink)
endif()
if (OpenS3D_USE_FFMPEG)
  add_subdirectory(core/ffmpeg)
endif()
if (OpenS3D_USE_CUDA AND OpenS3D_USE_CV)
  add_subdirectory(core/s3dcuda)
  add_subdirectory(apps/S3DCudaDemo)
endif()

if (OpenS3D_BUILD_APPS)
  if (OpenS3D_BUILD_OGRE_APPS)
    add_subdirectory(apps/OgreS3DPlayer)
  endif (OpenS3D_BUILD_OGRE_APPS)
  add_subdirectory(apps/FFmpegDecoding)
  add_subdirectory(apps/S3DDemo)
  if (OpenS3D_USE_CV)
    add_subdirectory(apps/S3DCVDemo)
    add_subdirectory(apps/S3DAnalyzer)
  endif()
endif()

# add test target (make test)
include(ProcessorCount)
ProcessorCount(N)
add_custom_target(
    Tests
    COMMAND make -j${N} && make CTEST_OUTPUT_ON_FAILURE=1 test)
