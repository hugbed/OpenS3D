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
 - Unit test coverage: [![Coverage Status](https://coveralls.io/repos/github/hugbed/OpenS3D/badge.svg?branch=master)](https://coveralls.io/github/hugbed/OpenS3D?branch=master)

## Coding Standards

This project uses clang-format Chromium coding standard.

## License

Interesting question, I'll come back to this later.
