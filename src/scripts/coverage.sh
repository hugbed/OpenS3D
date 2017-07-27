#!/usr/bin/env bash

# must be called inside the build directory
# project must be built with g++ with -DOpenS3D_BUILD_COVERAGE=ON

lcov --directory . --capture --output-file coverage.info # capture coverage info
lcov --remove coverage.info '/usr/*' --output-file coverage.info # filter out system
lcov --remove coverage.info 'src/libs/*' --output-file coverage.info # filter out libs
lcov --remove coverage.info 'test/*' --output-file coverage.info # filter out libs
lcov --remove coverage.info 'QtCore/*' --output-file coverage.info # filter out Qt
lcov --remove coverage.info 'QtGui/*' --output-file coverage.info # filter out Qt
lcov --remove coverage.info 'QtTest/*' --output-file coverage.info # filter out Qt
lcov --remove coverage.info 'QtWidgets/*' --output-file coverage.info # filter out Qt
lcov --list coverage.info #debug info

# generate HTML
# genhtml coverage.info --output-directory cov-html
