cmake_minimum_required(VERSION 3.2)

project(s3dcuda)

set(CUDA_TOOLKIT_ROOT_DIR /usr/local/cuda/)

find_package(CUDA QUIET REQUIRED)
find_package(OpenCV REQUIRED)

set(CUDA_PROPAGATE_HOST_FLAGS OFF)
set(CUDA_64_BIT_DEVICE_CODE ON)
set(CUDA_NVCC_FLAGS "${CUDA_NVCC_FLAGS} -std=c++11 -arch=sm_52" )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

include_directories(include)

file(GLOB_RECURSE SRC_FILES
        include/*.h
        src/*.cu
        src/*.cpp)

cuda_add_library(
    ${PROJECT_NAME}
    ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS} ${CUDA_LIBRARIES} s3d)
target_include_directories(${PROJECT_NAME} PUBLIC include)
