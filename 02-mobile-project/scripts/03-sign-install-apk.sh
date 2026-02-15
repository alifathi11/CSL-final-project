#!/bin/bash
set -e

SDK_PATH=/home/ali/Android/Sdk
BUILD_TOOLS=$SDK_PATH/build-tools/34.0.0

BUILD_DIR=build
KEYSTORE=release-key.jks
ALIAS=mykey
PASSWORD=123456

if [ ! -f "$KEYSTORE" ]; then
keytool -genkeypair \
    -alias $ALIAS \
    -keyalg RSA \
    -keysize 2048 \
    -validity 10000 \
    -keystore $KEYSTORE \
    -storepass $PASSWORD \
    -keypass $PASSWORD \
    -dname "CN=Ali, OU=Dev, O=MyGame, L=City, S=State, C=IR"
fi

echo "== Aligning APK =="
$BUILD_TOOLS/zipalign -p -f 4 \
    $BUILD_DIR/MyGame-unsigned.apk \
    $BUILD_DIR/MyGame-aligned.apk

echo "== Signing APK =="
$BUILD_TOOLS/apksigner sign \
    --ks $KEYSTORE \
    --ks-pass pass:$PASSWORD \
    --key-pass pass:$PASSWORD \
    --out $BUILD_DIR/MyGame.apk \
    $BUILD_DIR/MyGame-aligned.apk

echo "== Installing APK =="
adb install -r $BUILD_DIR/MyGame.apk

echo "== Done =="

