@echo off

set current_path=%~dp0

:: CMAKE_PREFIX_PATH
set CMAKE_PREFIX_PATH=C:\Qt\5.10.0\msvc2015

:: OpenCV
set OpenCV_DIR=%current_path%..\deps\opencv

:: PATH
set add_to_path=%current_path%..\deps\ffmpeg\x86
set add_to_path=%add_to_path%;%current_path%..\deps\ffmpeg\x86\bin
set add_to_path=%add_to_path%;%current_path%..\deps\opencv\x86\vc15\bin
set add_to_path=%add_to_path%;%CMAKE_PREFIX_PATH%\bin
set PATH=%PATH%;%add_to_path%
