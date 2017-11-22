#!/bin/sh

if [ "$CXX" == "" ]
then
    CXX=g++
fi

echo "Installing Qbs"

git clone --depth 1 -b v1.9.1 https://github.com/qbs/qbs.git qbs-build
pushd qbs-build
qmake -r qbs.pro QMAKE_CXX=$CXX QMAKE_LINK=$CXX && make -j3
export PATH=$PATH:$PWD/bin
popd