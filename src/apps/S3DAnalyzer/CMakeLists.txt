cmake_minimum_required(VERSION 3.2)

project ( S3DAnalyzer )

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# declaring files in your project

file(GLOB_RECURSE SRC_FILES
        controllers/*.cpp
        rendering/*.cpp
        widgets/*.cpp
        worker/*.cpp
        utilities/*.cpp)

set ( SOURCES
  ${SRC_FILES}
  main.cpp
  mainwindow.cpp
)

set ( MOC_HEADERS
 mainwindow.h
 widgets/settingswidget.h
)

set ( UIS
 mainwindow.ui
 widgets/settingswidget.ui
)

set ( RESOURCES
  resources.qrc
  style/style.qrc
)

# Next lines needed for building all Qt projects
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5 5.9 COMPONENTS Widgets REQUIRED )
find_package(OpenCV REQUIRED)

# using Qt meta-system (precompiler)
QT5_ADD_RESOURCES( RES_SOURCES ${RESOURCES} )
QT5_WRAP_UI( UI_HEADERS ${UIS} )

include_directories(
    ${CMAKE_SOURCE_DIR}/core/s3d/include
    ${CMAKE_SOURCE_DIR}/core/s3dcv/include
    ${CMAKE_SOURCE_DIR}/libs/gsl/include)

# compile
add_executable(${PROJECT_NAME} ${SOURCES} ${MOC_SRCS} ${RES_SOURCES} ${UI_HEADERS})
target_link_libraries(${PROJECT_NAME} s3d s3dcv Qt5::Widgets Qt5::Core Qt5::Gui pthread)

# tests
add_subdirectory(test)
