FROM ubuntu:16.04

RUN apt-get update && \
    apt-get install -y \
        git \
        curl \
        wget \
        unzip \
        build-essential \
        gcc-5 g++-5 \
        cmake \
        lcov \
        software-properties-common \
        pkg-config

# install ffmpeg
RUN add-apt-repository ppa:jonathonf/ffmpeg-3 -y && \
    apt-get update && \
    apt-get install ffmpeg libav-tools x264 x265 \
    libavcodec-dev libavformat-dev libavutil-dev libswscale-dev libavresample-dev libavcodec-dev -y

ENV CC /usr/bin/gcc 
ENV CXX /usr/bin/g++
ENV PATH "$PATH:/bin"

# install opencv 3.2
WORKDIR /
RUN wget https://github.com/Itseez/opencv/archive/3.2.0.zip \
    && unzip 3.2.0.zip \
    && mkdir /opencv-3.2.0/cmake_binary && cd /opencv-3.2.0/cmake_binary \
    && cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local .. \
    && make install \
    && rm /3.2.0.zip \
    && rm -r /opencv-3.2.0

# build and test OpenS3D
CMD cd /opt && \
    git clone https://github.com/hugbed/OpenS3D.git && \
    cd OpenS3D/src && git checkout -qf "$TRAVIS_COMMIT" && \
    mkdir build && cd build && \ 
    cmake .. \
        -DOpenS3D_BUILD_APPS=OFF \
        -DOpenS3D_USE_CUDA=OFF \
        -DOpenS3D_BUILD_COVERAGE=ON \ 
        -DOpenS3D_USE_CV=ON && \
    make -j2 && make Tests && \
    ../scripts/coverage.sh && \
    curl -s https://codecov.io/bash > .codecov && chmod +x .codecov && \
    bash ./.codecov -X gcov
