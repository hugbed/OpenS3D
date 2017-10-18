cmake_minimum_required(VERSION 3.2)

project(S3DCVDemo)

find_package(OpenCV REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

file(GLOB_RECURSE INCLUDE_FILES include/*.h)

include_directories(include ${GSL_INCLUDE_DIR})

add_executable(${PROJECT_NAME}_camera_alignment ${PROJECT_SOURCE_DIR}/src/camera_alignment_demo.cpp ${INCLUDE_FILES} src/test_disparity_analyzer_stan.cpp)
target_link_libraries(${PROJECT_NAME}_camera_alignment s3dcv ${OpenCV_LIBS} gsl)

add_executable(${PROJECT_NAME}_test_dataset_rectification ${PROJECT_SOURCE_DIR}/src/test_dataset_rectification.cpp ${INCLUDE_FILES})
target_link_libraries(${PROJECT_NAME}_test_dataset_rectification s3dcv ${OpenCV_LIBS} gsl)

add_executable(${PROJECT_NAME}_compare_ransac_matches ${PROJECT_SOURCE_DIR}/src/compare_ransac_matches.cpp ${INCLUDE_FILES})
target_link_libraries(${PROJECT_NAME}_compare_ransac_matches s3dcv ${OpenCV_LIBS} gsl)

add_executable(${PROJECT_NAME}_test_model_rectification_epilines ${PROJECT_SOURCE_DIR}/src/test_model_rectification_epilines.cpp ${INCLUDE_FILES})
target_link_libraries(${PROJECT_NAME}_test_model_rectification_epilines s3dcv ${OpenCV_LIBS} gsl)

add_executable(${PROJECT_NAME}_test_ransac_rectification ${PROJECT_SOURCE_DIR}/src/test_ransac_rectification.cpp ${INCLUDE_FILES})
target_link_libraries(${PROJECT_NAME}_test_ransac_rectification s3dcv ${OpenCV_LIBS} gsl)

add_executable(${PROJECT_NAME}_test_disparity_analyzer_stan ${PROJECT_SOURCE_DIR}/src/test_disparity_analyzer_stan.cpp ${INCLUDE_FILES})
target_link_libraries(${PROJECT_NAME}_test_disparity_analyzer_stan s3dcv ${OpenCV_LIBS} gsl)