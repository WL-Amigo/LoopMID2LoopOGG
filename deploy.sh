#!/bin/bash
BUILD_DIR=build_release
DEPLOY_DIR=dist
FULL_DEPLOY_DIR=dist-full

rm -rf $BUILD_DIR/
mkdir $BUILD_DIR/
rm -rf $DEPLOY_DIR/
mkdir $DEPLOY_DIR/
rm -rf $FULL_DEPLOY_DIR/

pushd $BUILD_DIR/
qmake ../GUI/LoopMID2LoopOGG.pro -spec win32-g++ -config release
mingw32-make -j8
popd

pushd $DEPLOY_DIR/
cp ../$BUILD_DIR/release/LoopMID2LoopOGG.exe ./
windeployqt --release LoopMID2LoopOGG.exe
rm -rf iconengines/ imageformats/ styles/ translations/
rm D3Dcompiler_47.dll Qt5Svg.dll
popd
cp -t $DEPLOY_DIR/ LICENSE LICENSE.gplv2 LICENSE.lgplv3 LICENSE.libogg LICENSE.libvorbis LICENSE.midifile
cp ./README.md $DEPLOY_DIR/README.txt
cp ./LICENSE.SGM $DEPLOY_DIR/LICENSE.SGM.txt
cp -r ./external-resources/TiMidity++/ $DEPLOY_DIR/
cp -r ./external-resources/qaac/ $DEPLOY_DIR/
cp ./external-resources/oggenc2.exe $DEPLOY_DIR/

if [[ $1 == "full" ]]; then
  cp -r $DEPLOY_DIR/ $FULL_DEPLOY_DIR
  cp -r ./external-resources/cfg/ $FULL_DEPLOY_DIR/
  mv $FULL_DEPLOY_DIR/cfg/ $FULL_DEPLOY_DIR/sf2/
  cp ./external-resources/SGM-V2.01.sf2 $FULL_DEPLOY_DIR/sf2/
fi