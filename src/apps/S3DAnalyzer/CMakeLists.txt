cmake_minimum_required(VERSION 3.2)

project ( S3DAnalyzer )

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# declaring files in your project

set ( SOURCES
  main.cpp
  mainwindow.cpp
  rendering/openglwindow.cpp
  rendering/openglwidget.cpp
  rendering/texturemanager.cpp
  rendering/renderingcontext.cpp
  rendering/entity/entitymanager.cpp
  rendering/entity/stereo/rectangleentity.cpp
  rendering/entity/stereo/anaglyphrectangleentity.cpp
  rendering/entity/stereo/stereosidebysideentity.cpp
  rendering/entity/stereo/opacityrectangleentity.cpp
  rendering/entity/stereo/interlacedrectangleentity.cpp
  rendering/entity/stereo/leftrectangleentity.cpp
  rendering/entity/stereo/rightrectangleentity.cpp
  rendering/entity/stereo/viewercentricentity.cpp
  rendering/entity/billboard/billboardentity.cpp
  rendering/entity/billboard/billboardcolorentity.cpp
  rendering/entity/billboard/billboarddisparityentity.cpp
  rendering/entity/billboard/billboarddepthentity.cpp
  widgets/parameterslistview.cpp
  widgets/parameterwidget.cpp
  widgets/doublespinboxnocursor.cpp
  widgets/lineeditnocursor.cpp
  widgets/widthhintlabel.cpp
  widgets/rangewidget.cpp
  widgets/rangebar.cpp
  widgets/videocontrols.cpp
  widgets/videobuttons.cpp
  widgets/sliderdirectjump.cpp
  widgets/settingswidget.cpp
  widgets/settingsdialog.cpp
  worker/videosynchronizer.cpp
  worker/stereo_demuxer_qimage.cpp
  worker/stereo_demuxer_qimage_side_by_side.cpp
  worker/stereo_demuxer_qimage_above_below.cpp
  worker/stereo_demuxer_factory_qimage.cpp
  utilities/depthintensitygradient.cpp)

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
