@echo off

call "set-environment.bat"
echo %OpenCV_DIR%
cd ..
mkdir build
cd build
cmake .. -G "Visual Studio 15 2017"

cd ../scripts