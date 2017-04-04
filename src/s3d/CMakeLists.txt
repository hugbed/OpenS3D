cmake_minimum_required(VERSION 3.5)

project(S3D)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

find_package(OpenCV REQUIRED)

#file(GLOB CPP_FILES src/*.cpp)

include_directories(include)

add_executable(DisparityDemo ${PROJECT_SOURCE_DIR}/src/disparity_demo.cpp)
target_link_libraries(DisparityDemo ${OpenCV_LIBS})

# Expose project's public includes to other
# subprojects through cache variable.
set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)
