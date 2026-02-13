#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <jni.h>

extern "C" {

struct RandInit {
    RandInit() { std::srand(std::time(nullptr)); }
} randInit;

float currentSpeedMultiplier = 1.0f;
float maxSpeedMultiplier     = 2.0f;
float speedIncreaseStep      = 0.1f;

JNIEXPORT void JNICALL
Java_com_example_game_GameView_nativeUpdate(
        JNIEnv* env,
        jobject thiz,
        jfloat screenWidth, 
        jfloat screenHeight,
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
        jfloat screenWidth,
        jfloat screenHeight, 
        jfloatArray posXArr,
        jfloatArray posYArr,
        jfloatArray velXArr,
        jfloatArray velYArr,
        jbooleanArray aliveArr,
        jint index,
        jfloat radius) 
{

    jint count = env->GetArrayLength(posXArr);
    if (index < 0 || index >= count) return;

    jfloat* posX = env->GetFloatArrayElements(posXArr, nullptr);
    jfloat* posY = env->GetFloatArrayElements(posYArr, nullptr);
    jfloat* velX = env->GetFloatArrayElements(velXArr, nullptr);
    jfloat* velY = env->GetFloatArrayElements(velYArr, nullptr);

    jboolean* alive = env->GetBooleanArrayElements(aliveArr, nullptr);

    if (currentSpeedMultiplier < maxSpeedMultiplier) {
        currentSpeedMultiplier = std::min(currentSpeedMultiplier + speedIncreaseStep, maxSpeedMultiplier);
        speedIncreaseStep *= 0.8f;
    }

    int side = std::rand() % 4;

    float w = screenWidth;
    float h = screenHeight;

    float baseSpeedX = ((float)std::rand() / (float)RAND_MAX) * 2000.f + 1000.f;
    float baseSpeedY = ((float)std::rand() / (float)RAND_MAX) * 2000.f + 1000.f;

    float speedX = baseSpeedX * currentSpeedMultiplier;
    float speedY = baseSpeedY * currentSpeedMultiplier;

    switch (side) {
        case 0: // top
            posX[index] = ((float)std::rand() / (float)RAND_MAX) * w;
            posY[index] = -radius;
            velX[index] = ((float)std::rand() / (float)RAND_MAX - 0.5f) * speedX;
            velY[index] = std::fabs(speedY);
            break;

        case 1: // bottom
            posX[index] = ((float)std::rand() / (float)RAND_MAX) * w;
            posY[index] = h + radius;
            velX[index] = ((float)std::rand() / (float)RAND_MAX - 0.5f) * speedX;
            velY[index] = -std::fabs(speedY);
            break;

        case 2: // left
            posX[index] = -radius;
            posY[index] = ((float)std::rand() / (float)RAND_MAX) * h;
            velX[index] = std::fabs(speedX);
            velY[index] = ((float)std::rand() / (float)RAND_MAX - 0.5f) * speedY;
            break;

        case 3: // right
            posX[index] = w + radius;
            posY[index] = ((float)std::rand() / (float)RAND_MAX) * h;
            velX[index] = -std::fabs(speedX);
            velY[index] = ((float)std::rand() / (float)RAND_MAX - 0.5f) * speedY;
            break;
    }

    alive[index] = true;

    env->ReleaseFloatArrayElements(posXArr, posX, 0);
    env->ReleaseFloatArrayElements(posYArr, posY, 0);
    env->ReleaseFloatArrayElements(velXArr, velX, 0);
    env->ReleaseFloatArrayElements(velYArr, velY, 0);
    env->ReleaseBooleanArrayElements(aliveArr, alive, 0);
}

JNIEXPORT void JNICALL
Java_com_example_game_GameView_nativeApplyGravity(
        JNIEnv* env,
        jobject thiz,
        jfloat screenWidth,
        jfloat screenHeight, 
        jfloatArray posXArr,
        jfloatArray posYArr,
        jfloatArray velXArr,
        jfloatArray velYArr,
        jbooleanArray aliveArr,
        jfloat dt,
        jfloat gravity,
        jfloat radius) 
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

        velY[i] += gravity * dt;
        posY[i] += velY[i] * dt;

        if (posY[i] > h - radius) {
            posY[i] = h - radius;
            velY[i] *= -0.3f;
        }
    }

    env->ReleaseFloatArrayElements(posXArr, posX, 0);
    env->ReleaseFloatArrayElements(posYArr, posY, 0);
    env->ReleaseFloatArrayElements(velXArr, velX, 0);
    env->ReleaseFloatArrayElements(velYArr, velY, 0);
    env->ReleaseBooleanArrayElements(aliveArr, alive, 0);
}

}
