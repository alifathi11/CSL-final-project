#include <cmath>
#include <jni.h>

extern "C" {

// prototype
JNIEXPORT void JNICALL
Java_com_example_game_GameView_nativeUpdate(
        JNIEnv* env,
        jobject thiz,
        jint screenWidth, 
        jint screenHeight,
        jfloatArray posXArr,
        jfloatArray posYArr,
        jfloatArray velXArr,
        jfloatArray velYArr,
        jbooleanArray aliveArr,
        jfloat radius,
        jfloat dt,
        jboolean gameOver)
{
    jint count = env->GetArrayLength(posXArr);

    jfloat* posX = env->GetFloatArrayElements(posXArr, nullptr);
    jfloat* posY = env->GetFloatArrayElements(posYArr, nullptr);
    jfloat* velX = env->GetFloatArrayElements(velXArr, nullptr);
    jfloat* velY = env->GetFloatArrayElements(velYArr, nullptr);

    jboolean* alive = env->GetBooleanArrayElements(aliveArr, nullptr);

    float w = screenWidth;
    float h = screenHeight;

    for (int i = 0; i < count; i++) {
        if (!alive[i]) continue;

        if (gameOver) {
            velY[i] += 2000.0f * dt;
        }

        posX[i] += velX[i] * dt;
        posY[i] += velY[i] * dt;

        if (!gameOver) {
            if (posX[i] < radius) { posX[i] = radius; velX[i] = -velX[i]; }
            if (posX[i] > w - radius) { posX[i] = w - radius; velX[i] = -velX[i]; }
            if (posY[i] < radius) { posY[i] = radius; velY[i] = -velY[i]; }
            if (posY[i] > h - radius) { posY[i] = h - radius; velY[i] = -velY[i]; }
        }
    }

    for (int i = 0; i < count; i++) {
        if (!alive[i]) continue;

        for (int j = i + 1; j < count; j++) {
            if (!alive[j]) continue;

            float dx = posX[j] - posX[i];
            float dy = posY[j] - posY[i];
            float distSq = dx * dx + dy * dy;
            float minDist = radius * 2.0f + 15.0f;

            if (distSq < minDist * minDist && distSq != 0.0f) {
                float dist = std::sqrt(distSq);
                float nx = dx / dist;
                float ny = dy / dist;
                float overlap = minDist - dist;

                posX[i] -= nx * overlap * 0.5f;
                posY[i] -= ny * overlap * 0.5f;
                posX[j] += nx * overlap * 0.5f;
                posY[j] += ny * overlap * 0.5f;

                float tmpVX = velX[i];
                float tmpVY = velY[i];
                velX[i] = velX[j];
                velY[i] = velY[j];
                velX[j] = tmpVX;
                velY[j] = tmpVY;

            }
        }
    }

    env->ReleaseFloatArrayElements(posXArr, posX, 0);
    env->ReleaseFloatArrayElements(posYArr, posY, 0);
    env->ReleaseFloatArrayElements(velXArr, velX, 0);
    env->ReleaseFloatArrayElements(velYArr, velY, 0);
    env->ReleaseBooleanArrayElements(aliveArr, alive, 0);
}
        
JNIEXPORT void JNICALL
Java_com_example_game_GameView_nativeRespawnBall(
        JNIEnv* env,
        jobject thiz,
        jint index,
        jfloat width,
        jfloat height,
        jfloat radius,
        jfloat currentSpeedMultiplier) 
{
    
}

}
