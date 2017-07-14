FROM ubuntu:16.04

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        git=1:2.7.4-0ubuntu1.1 \
        curl=7.47.0-1ubuntu2.2 \
        wget=1.17.1-1ubuntu1.2 \
        unzip=6.0-20ubuntu1 \
        build-essential=12.1ubuntu2 \
        gcc-5=5.4.0-6ubuntu1~16.04.4 \
        g++-5=5.4.0-6ubuntu1~16.04.4 \
        cmake=3.5.1-1ubuntu3 \
        lcov=1.12-2 \
        software-properties-common=0.96.20.7 \
        pkg-config=0.29.1-0ubuntu1 && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# install ffmpeg
RUN add-apt-repository ppa:jonathonf/ffmpeg-3 -y && \
    apt-get update && \
    apt-get install -y --no-install-recommends \ 
        ffmpeg=7:3.3.2-1~16.04.york1 \
        libav-tools=7:3.3.2-1~16.04.york1 \
        x264=2:0.148.2795+gitaaa9aa8-1~16.04.york0 \
        x265=2.4-1~16.04.york0 \
        libavcodec-dev=7:3.3.2-1~16.04.york1 \
        libavformat-dev=7:3.3.2-1~16.04.york1 \
        libavutil-dev=7:3.3.2-1~16.04.york1 \
        libswscale-dev=7:3.3.2-1~16.04.york1 \
        libavresample-dev=7:3.3.2-1~16.04.york1 && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

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
