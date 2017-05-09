## Open Source Library and Software for Stereoscopic 3D Content Production

The goal of this library is to provide users with tools (C++ library or sample applications) to create stereoscopic 3D content.
These tools should be sufficient to create 3D content either for stereoscopic cinema content production or depth analysis of 3D content such as virtual reality content.

This codebase is a work in process and work has just been started. It should get bigger as days go by.

### Features

Up to now: 
* 3D video player (side-by-side/anaglyph) with Ogre. To produce 3D content, you need to be able to visualize it.

### Build Instructions

This project uses C++14 and CMake:

```
mkdir build
cd build
make
```

Continuous integration:
 - linux 64 bits, GCC (Build + tests): [![Build Status](https://travis-ci.org/hugbed/OpenS3D.svg?branch=master)](https://travis-ci.org/hugbed/OpenS3D)
 - Unit test coverage: [![codecov](https://codecov.io/gh/hugbed/OpenS3D/branch/master/graph/badge.svg)](https://codecov.io/gh/hugbed/OpenS3D)

## Coding Standards

This project uses clang-format Chromium coding standard.

## License

This software is licensed under a BSD 3-clause license. See [LICENSE](LICENSE) file.
