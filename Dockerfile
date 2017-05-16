FROM ubuntu:16.04

RUN apt-get update && \
    apt-get install -y \
        git \
        build-essential \
        gcc-5 g++-5 \
        cmake \
        lcov

ENV CC /usr/bin/gcc 
ENV CXX /usr/bin/g++

WORKDIR /opt
RUN git clone https://github.com/hugbed/OpenS3D.git

RUN cd OpenS3D/src && mkdir build && cd build && \ 
    cmake .. -DOpenS3D_BUILD_APPS=OFF -DOpenS3D_USE_CUDA=OFF -DOpenS3D_BUILD_COVERAGE=ON && \
    make -j2 && make test

# Coverage - Uploading report to CodeCov
RUN ../scripts/coverage.sh && \
    bash < $(curl -s https://codecov.io/bash) -X gcov || echo "Codecov did not collect coverage reports"

