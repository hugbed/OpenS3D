#!/usr/bin/env bash

sudo apt-get -y install wget unzip
wget https://bitbucket.org/sinbad/ogre/get/v1-10-4.zip
unzip v1-10-4.zip
cd sinbad-ogre-1fc934daba1e
mkdir build && cd build
cmake ..
make && make install
