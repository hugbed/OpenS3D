## Dependencies Setup (Windows)

The project was tested on Windows 10 with Visual Studio 2017. Currently only s3d, s3d_cv and s3d_ffmpeg build on Windows. Plans are to implement s3d_decklink for Windows to support S3DAnalyzer which depends on all core (s3d) libraries.

### OpenCV

clone [OpenCV Github](https://github.com/opencv/opencv.git)  
clone [OpenCV Contrib Github](https://github.com/opencv/opencv_contrib.git)  

```batch
cd opencv
mkdir build
cd build
cmake .. -DOPENCV_EXTRA_MODULES_PATH=<opencv_contrib>/modules -G "Visual Studio 15 2017" :: use the compiler you wish to use here
```

Build OpenCV using Visual Studio in Debug + Release.
Building `INSTALL` project will create an `install` folder which you can copy directly to `<OpenS3D>/src/deps/opencv`.

### FFmpeg

The procedure was inspired from [this one](https://github.com/snikulov/ffmpeg_win_example/wiki/Full-build-procedure).

1- Download the "dev" and "shared" x86 versions from [here](https://ffmpeg.zeranoe.com/builds/).
2- Create a `<OpenS3D>/src/deps/ffmpeg` folder.
3- Copy the `bin` folder from the "shared" FFMPEG into `<OpenS3D>/src/deps/ffmpeg`.
4- Copy the `include` and `lib` folder from the "dev" FFMPEG into `<OpenS3D>/src/deps/ffmpeg`.

### Qt

1- Download [Qt](https://www1.qt.io/download-open-source/)
2- Install Qt from downloaded executable for your compiler (to C:/Qt)