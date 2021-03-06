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
  widgets/disparitysettingswidget.h
)

set ( UIS
  mainwindow.ui
  widgets/disparitysettingswidget.ui
  widgets/featuressettingsdialog.ui
  widgets/filtersettingsdialog.ui
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
    controllers
    rendering
    widgets
    worker
    utilities
    ${CMAKE_SOURCE_DIR}/core/s3d/include
    ${CMAKE_SOURCE_DIR}/core/s3dcv/include
    ${CMAKE_SOURCE_DIR}/libs/gsl/include)

# libraries to link
set(LINK_LIBS s3d s3d_cv s3d_ffmpeg Qt5::Widgets Qt5::Core Qt5::Gui)
if(UNIX AND NOT APPLE)
  # decklink is currently only supported on Linux
  set(LINK_LIBS ${LINK_LIBS} s3d_decklink)
endif()

# compile
add_executable(${PROJECT_NAME} ${SOURCES} ${MOC_SRCS} ${RES_SOURCES} ${UI_HEADERS})
target_link_libraries(${PROJECT_NAME} ${LINK_LIBS})

# tests
add_subdirectory(test)
