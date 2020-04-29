#!/bin/bash
export VERSION="1.3.2"
BUILD_DIR=build_release
DEPLOY_DIR=dist

mkdir $BUILD_DIR
mkdir $DEPLOY_DIR

pushd $BUILD_DIR
qmake ../sources/GUI/LoopMID2LoopOGG.pro -spec linux-g++ -config release
make -j8
make INSTALL_ROOT=appdir -j8 install
linuxdeployqt ./appdir/usr/share/applications/loopmid2loopogg.desktop -appimage
cp LoopMID2LoopOGG-$VERSION-x86_64.AppImage ~/sources/
popd