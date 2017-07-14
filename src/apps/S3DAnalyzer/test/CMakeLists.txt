find_package(Qt5Test REQUIRED )

include_directories(../)

set(TEST_ADDITIONAL_LIBRARIES Qt5::Widgets s3d)

set(TEST_ADDITIONAL_SOURCES
    ../widgets/lineeditnocursor.cpp
    ../rendering/entity/stereo/viewercentricentity.cpp
    ../utilities/depthintensitygradient.cpp)

include(${CMAKE_SOURCE_DIR}/cmake/qttest-cxx-dev-tools.cmake)
