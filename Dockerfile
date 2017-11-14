FROM ubuntu:16.04

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        git \
        curl \
        wget \
        unzip \
        build-essential \
        gcc-5 \
        g++-5 \
        cmake \
        lcov \
        software-properties-common \
        pkg-config && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# install ffmpeg
RUN add-apt-repository ppa:jonathonf/ffmpeg-3 -y && \
    apt-get update && \
    apt-get install -y --no-install-recommends \ 
        ffmpeg \
        libav-tools \
        x264 \
        x265 \
        libavcodec-dev \
        libavformat-dev \
        libavutil-dev \
        libswscale-dev \
        libavresample-dev && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

ENV CC /usr/bin/gcc 
ENV CXX /usr/bin/g++
ENV PATH "$PATH:/bin"

# install opencv 3.2
WORKDIR /
RUN wget https://github.com/opencv/opencv_contrib/archive/3.2.0.zip -O opencv_contrib-3.2.0.zip \
    && unzip opencv_contrib-3.2.0.zip
RUN wget https://github.com/Itseez/opencv/archive/3.2.0.zip \
    && unzip 3.2.0.zip \
    && mkdir /opencv-3.2.0/cmake_binary && cd /opencv-3.2.0/cmake_binary \
    && cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local \
             -D OPENCV_EXTRA_MODULES_PATH=/opencv_contrib-3.2.0/modules .. \
    && make install \
    && rm /3.2.0.zip \
    && rm -r /opencv-3.2.0

# build and test OpenS3D
CMD cd /opt && \
    git config --global user.email "hugbed@docker.com" && \
    git config --global user.name "Travis-Docker" && \
    git clone https://github.com/hugbed/OpenS3D.git && \
    cd OpenS3D/src &&  \
    echo $(if [ "$TRAVIS_PULL_REQUEST" != "false" ]; then git checkout -b build origin/"$TRAVIS_BRANCH" && \
                                                     git merge origin/"$TRAVIS_PULL_REQUEST_BRANCH" --no-ff --no-commit; fi) && \
    echo $(if [ "$TRAVIS_PULL_REQUEST" = "false" ]; then git checkout -qf "$TRAVIS_COMMIT"; fi) && \
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
