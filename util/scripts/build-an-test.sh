#!/bin/sh
QT_VERSION=v5.9.5-1

TARGET=qst
PLATFORM=mingw-w64
QBS_PROFILE=qt5
QBS_OPTIONS="project.version:test"

cd ../../
docker run -v ${PWD}:/build rweickelt/qt:${QT_VERSION}-${PLATFORM} install \
              --file /build/qst-project.qbs \
              --build-directory /build/build-$QBS_PROFILE-windows \
              --install-root /build/qst \
              release \
              profile:$QBS_PROFILE \
              $QBS_OPTIONS


TARGET=qst
PLATFORM=linux-x86_64
QBS_PROFILE=qt5
QBS_OPTIONS="project.runAutotest:true project.version:$QST_VERSION"
docker run -v ${PWD}:/build rweickelt/qt:${QT_VERSION}-${PLATFORM} install \
              --file /build/qst-project.qbs \
              --build-directory /build/build-$QBS_PROFILE-linux \
              --install-root /build/qst \
              release \
              profile:$QBS_PROFILE \
              $QBS_OPTIONS