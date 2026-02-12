#!/bin/bash
set -e
SDK_PATH=/home/ali/Android/Sdk
BUILD_DIR=build

mkdir -p $BUILD_DIR/classes

javac -source 1.8 -target 1.8 -bootclasspath $SDK_PATH/platforms/android-34/android.jar \
    -d $BUILD_DIR/classes ../app/src/main/java/com/example/game/MainActivity.java

cd $BUILD_DIR/classes
jar cf ../classes.jar *
cd ../..

$SDK_PATH/build-tools/34.0.0/d8 --lib $SDK_PATH/platforms/android-34/android.jar \
    --output $BUILD_DIR build/classes.jar

$SDK_PATH/build-tools/34.0.0/aapt package -f -m \
    -M ../app/src/main/AndroidManifest.xml \
    -S ../app/src/main/res \
    -I $SDK_PATH/platforms/android-34/android.jar \
    -F $BUILD_DIR/MyGame-unsigned.apk

zip -j $BUILD_DIR/MyGame-unsigned.apk $BUILD_DIR/classes.dex

mkdir -p tmp/lib/arm64-v8a
cp $BUILD_DIR/libgame.so tmp/lib/arm64-v8a/
cd tmp
zip -r ../$BUILD_DIR/MyGame-unsigned.apk lib
cd ..
rm -rf tmp

