cmake_minimum_required(VERSION 3.5)

project(S3DDemo)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

find_package(OpenCV REQUIRED)

add_executable(${PROJECT_NAME}_Disparity ${PROJECT_SOURCE_DIR}/src/disparity_demo.cpp)
target_link_libraries(${PROJECT_NAME}_Disparity ${OpenCV_LIBS} S3D)