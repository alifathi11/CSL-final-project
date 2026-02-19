#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <jni.h>

extern "C" {

// Random seed initializer to ensure different random values each run
struct RandInit {
    RandInit() { std::srand(std::time(nullptr)); }
} randInit;

// Speed control variables
const float maxSpeedMultiplier = 2.0f;

float currentSpeedMultiplier = 1.0f;
float speedIncreaseStep      = 0.1f;

// Hit/Miss counters 
const int winHitCount   = 10;
const int loseMissCount = 10;

int hitCount  = 0;
int missCount = 0;

// Game State variables
bool gameOver  = false;
bool playerWon = false;

// Declaration of the assembly functions 
void update_positions_asm(
    float *posX,
    float *posY,
    const float *velX,
    const float *velY,
    int count,
    float dt,
    float radius, 
    float w,
    float h
);

void handle_wall_collisions_asm(
    float *posX,
    float *posY,
    float *velX,
    float *velY,
    int count,
    float radius,
    float w,
    float h,
    bool gameOver
);

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
        jfloat radius,
        jfloat dt,
        jboolean gameOver)
{
    jint count = env->GetArrayLength(posXArr);

    jfloat* posX = env->GetFloatArrayElements(posXArr, nullptr);
    jfloat* posY = env->GetFloatArrayElements(posYArr, nullptr);
    jfloat* velX = env->GetFloatArrayElements(velXArr, nullptr);
    jfloat* velY = env->GetFloatArrayElements(velYArr, nullptr);

    float w = screenWidth;
    float h = screenHeight;

    update_positions_asm(posX, posY, velX, velY, count, dt, radius, w, h);

    handle_wall_collisions_asm(posX, posY, velX, velY, count, radius, w, h, gameOver);

    for (int i = 0; i < count; i++) {

        for (int j = i + 1; j < count; j++) {

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
        jint index,
        jfloat radius) 
{

    jint count = env->GetArrayLength(posXArr);
    if (index < 0 || index >= count) return;

    jfloat* posX = env->GetFloatArrayElements(posXArr, nullptr);
    jfloat* posY = env->GetFloatArrayElements(posYArr, nullptr);
    jfloat* velX = env->GetFloatArrayElements(velXArr, nullptr);
    jfloat* velY = env->GetFloatArrayElements(velYArr, nullptr);

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

    env->ReleaseFloatArrayElements(posXArr, posX, 0);
    env->ReleaseFloatArrayElements(posYArr, posY, 0);
    env->ReleaseFloatArrayElements(velXArr, velX, 0);
    env->ReleaseFloatArrayElements(velYArr, velY, 0);
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
        jfloat dt,
        jfloat gravity,
        jfloat radius) 
{
    jint count = env->GetArrayLength(posXArr);

    jfloat* posX = env->GetFloatArrayElements(posXArr, nullptr);
    jfloat* posY = env->GetFloatArrayElements(posYArr, nullptr);
    jfloat* velX = env->GetFloatArrayElements(velXArr, nullptr);
    jfloat* velY = env->GetFloatArrayElements(velYArr, nullptr);

    float w = screenWidth;
    float h = screenHeight; 

    // TODO: should be moved to asm
    for (int i = 0; i < count; i++) {

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
}

JNIEXPORT void JNICALL
Java_com_example_game_GameView_nativeRestartGame(
        JNIEnv* env,
        jobject thiz,
        jfloat screenWidth,
        jfloat screenHeight, 
        jfloatArray posXArr,
        jfloatArray posYArr,
        jfloatArray velXArr,
        jfloatArray velYArr
)
{
    jint count = env->GetArrayLength(posXArr);

    jfloat* posX = env->GetFloatArrayElements(posXArr, nullptr);
    jfloat* posY = env->GetFloatArrayElements(posYArr, nullptr);
    jfloat* velX = env->GetFloatArrayElements(velXArr, nullptr);
    jfloat* velY = env->GetFloatArrayElements(velYArr, nullptr);


    float w = screenWidth;
    float h = screenHeight; 

    currentSpeedMultiplier = 1.0f;
    speedIncreaseStep      = 0.1f;
    
    hitCount  = 0;
    missCount = 0;

    playerWon = false;
    gameOver  = false;
    
    float randomFloat; 

    // TODO: should be moved to asm
    for (int i = 0; i < count; i++) {
        randomFloat = ((float)std::rand() / (float)RAND_MAX);
        posX[i] = randomFloat * w;
        randomFloat = ((float)std::rand() / (float)RAND_MAX);
        posY[i] = randomFloat * h;
        randomFloat = ((float)std::rand() / (float)RAND_MAX);
        velX[i] = (randomFloat - 0.5f) * 4000;
        randomFloat = ((float)std::rand() / (float)RAND_MAX);
        velY[i] = (randomFloat - 0.5f) * 4000;
    }

    env->ReleaseFloatArrayElements(posXArr, posX, 0);
    env->ReleaseFloatArrayElements(posYArr, posY, 0);
    env->ReleaseFloatArrayElements(velXArr, velX, 0);
    env->ReleaseFloatArrayElements(velYArr, velY, 0);
}

JNIEXPORT int JNICALL
Java_com_example_game_GameView_nativeHandleTouch(
        JNIEnv* env,
        jobject thiz,
        jfloat touchX,
        jfloat touchY,
        jfloatArray posXArr,
        jfloatArray posYArr,
        jfloat radius
) 
{
    jint count = env->GetArrayLength(posXArr);

    jfloat* posX = env->GetFloatArrayElements(posXArr, nullptr);
    jfloat* posY = env->GetFloatArrayElements(posYArr, nullptr);

    bool hitSomething = false;

    float touchPadding = 75.0f;
    float touchRadius = radius + touchPadding;
    float touchRadiusSq = touchRadius * touchRadius;

    int ret = -1;

    // TODO: should be moved to asm
    for (int i = 0; i < count; i++) {

        float dx = touchX - posX[i];
        float dy = touchY - posY[i];

        if (dx * dx + dy * dy <= touchRadiusSq) {

            hitSomething = true;
            hitCount++;

            ret = i;
            break;
        }
    }

    if (!hitSomething) {
        missCount++;
        ret = -1;
    }
     
    env->ReleaseFloatArrayElements(posXArr, posX, 0);
    env->ReleaseFloatArrayElements(posYArr, posY, 0);

    return ret;
}

JNIEXPORT void JNICALL
Java_com_example_game_GameView_nativeCheckEnd() 
{
    if (hitCount >= winHitCount) {
        gameOver  = true;
        playerWon = true;
    } else if (missCount >= loseMissCount) {
        gameOver  = true;
        playerWon = false;
    }
}

JNIEXPORT int JNICALL 
Java_com_example_game_GameView_nativeGetHitCount() 
{
    return hitCount;
}

JNIEXPORT int JNICALL 
Java_com_example_game_GameView_nativeGetMissCount() 
{
    return missCount;
}

JNIEXPORT jboolean JNICALL 
Java_com_example_game_GameView_nativeGetGameOver()
{
    return gameOver;
}

JNIEXPORT jboolean JNICALL 
Java_com_example_game_GameView_nativeGetPlayerWon()
{
    return playerWon;
}

}        