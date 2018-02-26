[![Build Status](https://travis-ci.org/hugbed/OpenS3D.svg?branch=master)](https://travis-ci.org/hugbed/OpenS3D) [![Codacy Badge](https://api.codacy.com/project/badge/Grade/b1f285e4d9f34686910f4cc100c6f56d)](https://www.codacy.com/app/hugbed/OpenS3D?utm_source=github.com&utm_medium=referral&utm_content=hugbed/OpenS3D&utm_campaign=badger) [![codecov](https://codecov.io/gh/hugbed/OpenS3D/branch/master/graph/badge.svg)](https://codecov.io/gh/hugbed/OpenS3D)

## Open-Source Real-Time Assistance Framework for Stereoscopic Content Production

The goal of this library is to provide users with tools (C++ library and software) to create stereoscopic 3D content.
These tools should be sufficient to create stereoscopic 3D for cinematic purposes.

![S3DAnalyzer](https://i.imgur.com/BnjfVFd.png)

## Features

Stereoscopic 3D content analysis is available through a modern C++ library.

Most algorithms are implemented using OpenCV but the library uses interfaces so that it could be implemented with any other computer vision frameworks.

The usage of the library is demonstrated with a stereoscopic 3D analysis software (S3DAnalyzer):

* 3D Video player (anaglyph, side-by-side)
  * Side-by-side
  * Above-below
  * Separate files (left, right)
  * Supports many video formats and codecs (with FFmpeg)
  * Variable horizontal image translation to adjust the image plane (useful for raw videos for which all points are projected in front of the screen).
* Camera alignment
  * Detect misalignments (roll angle, vertical offset, zoom ratio, tilt, pan, etc.)
* Camera rectification
  * Correct misalignments detected to minimize vertical disparities
* Disparity analysis
  * Disparity range
  * Colored feature points from disparity (red = too close in front of the screen, purple = too far behind screen plane)
* Viewer-centric depth analysis
  * Depth from screen plane of detected features (from viewer distance, screen size...)

## Build Instructions

This project uses C++14 and CMake.

This project is mainly developped on Linux (Ubuntu, Arch) but has recently been ported to Mac OS and Windows (see [Windows Setup](./windows-setup.md)). FFmpeg can be installed from source (or with brew on Mac OS) and OpenCV compiled from source.

Dependencies are:

* FFmpeg >= 3.0
* OpenCV >= 3.0
* Qt >= 5.7

If all dependencies are correctly configured, the project can be built with cmake:

```
mkdir build
cd build
make
```

Compilers:
 - Continuous development: linux 64 bits, GCC (Build + Tests)
 - Internal development: linux 64 bits, Clang (Build + Tests)

## Coding Standards

This project uses clang-format Chromium coding standard.

## License

This software is licensed under a BSD 3-clause license. See [LICENSE](LICENSE) file.

## Citations

Cite as:

```
@misc{bedard2018opens3d,
  author = "Hugo Bédard",
  title = "OpenS3D, an Open-Source Real-Time Assistance Framework for Stereoscopic Content Production",
  howpublished = "\url{https://github.com/hugbed/OpenS3D}",
}
```
