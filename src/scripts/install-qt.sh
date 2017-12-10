#!/bin/bash

# install dependencies
apt-get install mesa-common-dev libglu1-mesa-dev build-essential libfontconfig1 -y

# download + run installer (choose install in /opt/Qt)
version=5.9.1
filename=qt-opensource-linux-x64-$version.run
wget http://download.qt.io/official_releases/qt/${version:0:3}/5.9.1/$filename
chmod +x $filename
./$filename
rm $filename

# add user rights to Qt directory
chown -R $USER:$USER /opt/Qt 
echo export CMAKE_PREFIX_PATH=/opt/Qt/$version/gcc_64/:$CMAKE_PREFIX_PATH >> ~/.profile
