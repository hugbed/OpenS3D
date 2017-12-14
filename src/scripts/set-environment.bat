@echo off

set current_path=%~dp0

:: PATH
set add_to_path=%current_path%
set add_to_path=%add_to_path%;..\deps\ffmpeg\x86
set add_to_path=%add_to_path%;..\deps\opencv
set PATH=%PATH%;%add_to_path%

:: CMAKE_PREFIX_PATH
set CMAKE_PREFIX_PATH=C:/Qt/5.10.0/msvc2017_64

:: OpenCV
set OpenCV_DIR=%current_path%..\deps\opencv


rem set OPENCV_DIR=D:\OpenCV\Build\x86\vc10     (suggested for Visual Studio 2010 - 32 bit Windows)
rem set OPENCV_DIR=D:\OpenCV\Build\x64\vc10     (suggested for Visual Studio 2010 - 64 bit Windows)
rem set OPENCV_DIR=D:\OpenCV\Build\x86\vc11     (suggested for Visual Studio 2012 - 32 bit Windows)
rem set OPENCV_DIR=D:\OpenCV\Build\x64\vc11     (suggested for Visual Studio 2012 - 64 bit Windows)