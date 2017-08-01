# QtOpenGLSample

## Stereoscopic 3D Analyzer

Using Qt 5 and OpenGL for fast image update / video streaming.

Display modes are implemented with shaders, images are rendered on a rectangle mesh with the current images
as textures.

## Features

* Different stereo 3D display modes
* Camera alignment parameters
* Horizontal image translation adjustment
* Convergence indicator
* Viewer-Centric display

## Preview

![Preview](http://imgur.com/iVtXtv3)

## Building

Make sure that the right Qt version (>= 5.9) is linked using cmake.
Failure to do so may lead to QPainter not working well with OpenGL rendering.
For example, if Qt is installed in /opt/Qt, could do add this line 
to its ~/.bashrc or other file that is sourced:

```
export CMAKE_PREFIX_PATH=/opt/Qt/5.9/gcc_64:$CMAKE_PREFIX_PATH
```

