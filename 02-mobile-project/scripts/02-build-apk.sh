#!/bin/bash
set -e

SDK_PATH=/home/ali/Android/Sdk
BUILD_TOOLS=$SDK_PATH/build-tools/34.0.0
ANDROID_JAR=$SDK_PATH/platforms/android-34/android.jar

APP_DIR=../app/src/main
CPP_BUILD_DIR=$APP_DIR/cpp/build
BUILD_DIR=build

echo "== Cleaning build directory =="
rm -rf $BUILD_DIR
mkdir -p $BUILD_DIR/classes
mkdir -p $BUILD_DIR/gen

echo "== Generating R.java =="
$BUILD_TOOLS/aapt package -f -m \
    -J $BUILD_DIR/gen \
    -M $APP_DIR/AndroidManifest.xml \
    -S $APP_DIR/res \
    -I $ANDROID_JAR

echo "== Compiling Java sources =="
javac \
    $(find $APP_DIR/java -name "*.java") \
    $(find $BUILD_DIR/gen -name "*.java") \
    -d $BUILD_DIR/classes \
    -classpath $ANDROID_JAR

echo "== Creating classes.jar =="
cd $BUILD_DIR/classes
jar cf ../classes.jar *
cd ../..

echo "== Converting to DEX =="
$BUILD_TOOLS/d8 \
    --lib $ANDROID_JAR \
    --output $BUILD_DIR \
    $BUILD_DIR/classes.jar

echo "== Packaging unsigned APK =="
$BUILD_TOOLS/aapt package -f \
    -M $APP_DIR/AndroidManifest.xml \
    -S $APP_DIR/res \
    -I $ANDROID_JAR \
    -F $BUILD_DIR/MyGame-unsigned.apk

echo "== Adding classes.dex =="
zip -j $BUILD_DIR/MyGame-unsigned.apk $BUILD_DIR/classes.dex

echo "== Adding native library libgame.so =="

# Automatically locate libgame.so
SO_PATH=$(find $CPP_BUILD_DIR -name libgame.so)

if [ -z "$SO_PATH" ]; then
    echo "ERROR: libgame.so not found!"
    exit 1
fi

mkdir -p tmp/lib/arm64-v8a
cp $SO_PATH tmp/lib/arm64-v8a/

cd tmp
zip -r ../$BUILD_DIR/MyGame-unsigned.apk lib
cd ..
rm -rf tmp

echo "== Build finished successfully =="

