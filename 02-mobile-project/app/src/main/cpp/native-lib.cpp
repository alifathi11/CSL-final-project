#include <jni.h>
#include <android/log.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "GameTest", __VA_ARGS__))

extern "C" JNIEXPORT void JNICALL
Java_com_example_game_MainActivity_testNative(JNIEnv *, jobject) {
    LOGI("Native library loaded and working!");
}

