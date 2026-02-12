#!/bin/bash
set -e
BUILD_DIR=build
KEYSTORE=release-key.jks
ALIAS=mykey
PASSWORD=123456
if [ ! -f "$KEYSTORE" ]; then
keytool -genkeypair -alias $ALIAS -keyalg RSA -keysize 2048 -validity 10000 -keystore $KEYSTORE -storepass $PASSWORD -keypass $PASSWORD -dname "CN=Ali, OU=Dev, O=MyGame, L=City, S=State, C=IR"
fi
/home/ali/Android/Sdk/build-tools/34.0.0/apksigner sign --ks $KEYSTORE --ks-pass pass:$PASSWORD --key-pass pass:$PASSWORD --out $BUILD_DIR/MyGame.apk $BUILD_DIR/MyGame-unsigned.apk
adb install -r $BUILD_DIR/MyGame.apk

