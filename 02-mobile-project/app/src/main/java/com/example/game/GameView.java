package com.example.game;

import java.util.Random;

import android.content.Context;
import android.view.MotionEvent;
import android.view.View;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Color;

import android.media.SoundPool;
import android.media.AudioAttributes;

public class GameView extends View {

    // Load static lib
    static {
        System.loadLibrary("game"); 
    }

    // Gravity 
    private float gravity = 2000f;

    // Balls' Count
    private final int ballCount = 4;

    // Balls' Radius
    private final float radius = 100f;

    // Balls' Positions 
    private float[] posX;
    private float[] posY;
    private float[] velX;
    private float[] velY;
    
    // Paints
    private final Paint ballPaint; 
    private final Paint textPaintWhite;
    private final Paint textPaintRed;
    private final Paint endPaint;

    // Times 
    private long lastTime;

    // Sounds
    private SoundPool soundPool;
    private final int popSoundId;

    private boolean soundLoaded = false;

    public GameView(Context context) {
        super(context);

        // Paints
        ballPaint = new Paint();
        ballPaint.setColor(Color.GREEN);

        textPaintWhite = new Paint();
        textPaintWhite.setColor(Color.WHITE);
        textPaintWhite.setTextSize(50f);
        textPaintWhite.setAntiAlias(true);

        textPaintRed = new Paint();
        textPaintRed.setColor(Color.RED);
        textPaintRed.setTextSize(50f);
        textPaintRed.setAntiAlias(true);

        endPaint = new Paint();
        endPaint.setTextSize(120f);
        endPaint.setTextAlign(Paint.Align.CENTER);
        endPaint.setAntiAlias(true);

        // Balls' Positions
        posX = new float[ballCount];
        posY = new float[ballCount];
        velX = new float[ballCount];
        velY = new float[ballCount];

        Random rnd = new Random();
        for (int i = 0; i < ballCount; i++) {
            posX[i] = rnd.nextFloat() * 800;
            posY[i] = rnd.nextFloat() * 1200;
            velX[i] = (rnd.nextFloat() - 0.5f) * 2000;
            velY[i] = (rnd.nextFloat() - 0.5f) * 2000;
        }

        // Times
        lastTime = System.nanoTime();

        // Sounds
        AudioAttributes audioAttributes = new AudioAttributes.Builder()
            .setUsage(AudioAttributes.USAGE_GAME)
            .setContentType(AudioAttributes.CONTENT_TYPE_SONIFICATION)
            .build();

        soundPool = new SoundPool.Builder()
            .setMaxStreams(5)
            .setAudioAttributes(audioAttributes)
            .build();

        popSoundId = soundPool.load(context, R.raw.pop, 1);

        soundPool.setOnLoadCompleteListener((sp, sampleId, status) -> {
            if (status == 0) {
                soundLoaded = true;
            }
        });

    }

    private float updateTime() {
        long now = System.nanoTime();
        float dt = (now - lastTime) / 1_000_000_000f;
        if (dt > 0.033f) dt = 0.033f; 
        lastTime = now;

        return dt;
    }

    private void drawBackground(Canvas canvas) {
        canvas.drawColor(Color.BLACK);
    }

    private void drawBalls(Canvas canvas) {
        for (int i = 0; i < ballCount; i++) {
            canvas.drawCircle(posX[i], posY[i], radius, ballPaint);
        }
    }

    private void drawHitAndMissCount(Canvas canvas) {
        int hitCount  = nativeGetHitCountWrapper();
        int missCount = nativeGetMissCountWrapper();
        
        canvas.drawText("Hits: " + hitCount, 30f, 60f, textPaintWhite);
        canvas.drawText("Miss: " + missCount, 30f, 120f, textPaintRed);
    }

    private void drawEndGameScreen(Canvas canvas) {

        float cx = getWidth()  / 2f;
        float cy = getHeight() / 2f;

        boolean playerWon = nativeGetPlayerWonWrapper();

        endPaint.setColor(playerWon ? Color.GREEN : Color.RED);
        String msg = playerWon ? "You Win!" : "Game Over";
        canvas.drawText(msg, cx, cy, endPaint);
    }

    @Override 
    protected void onDraw(Canvas canvas) {

        // Update Time
        float dt = updateTime();
        
        // Update Game 
        updateGame(dt);
        
        // Draw Background
        drawBackground(canvas);

        // Draw Balls
        drawBalls(canvas);

        // Draw Hit/Miss Count
        drawHitAndMissCount(canvas);

        // Draw Endgame Screen 

        boolean gameOver = nativeGetGameOverWrapper();
        if (gameOver) 
            drawEndGameScreen(canvas);

        postInvalidateOnAnimation();
    }

    private void updateGame(float dt) {

        boolean gameOver = nativeGetGameOverWrapper();

        if (gameOver) {
            nativeApplyGravityWrapper(dt);
        } else {
            nativeUpdateWrapper(dt);
            nativeCheckEndWrapper();
        }
    }

    @Override 
    public boolean onTouchEvent(MotionEvent event) {

        if (event.getAction() != MotionEvent.ACTION_DOWN)
            return true;

        boolean gameOver = nativeGetGameOverWrapper();

        if (gameOver) {
            nativeRestartGameWrapper();
            return true;
        }

        float touchX = event.getX();
        float touchY = event.getY();

        int hitIndex = nativeHandleTouchWrapper(touchX, touchY);

        if (hitIndex != -1) {

            nativeRespawnBallWrapper(hitIndex);

            if (soundLoaded) {
                float rate = 0.9f + new Random().nextFloat() * 0.2f;
                soundPool.play(popSoundId, 1f, 1f, 1, 0, rate);
            }
        }

        return true;
    }

    @Override
    protected void onDetachedFromWindow() {
        super.onDetachedFromWindow();
        if (soundPool != null) {
            soundPool.release();
            soundPool = null;
        }
    }

    private void nativeUpdateWrapper(float dt) {
        nativeUpdate(getWidth(), getHeight(), posX, posY, velX, velY, radius, dt);
    }

    private void nativeRespawnBallWrapper(int i) {
        nativeRespawnBall(getWidth(), getHeight(), posX, posY, velX, velY, i, radius);
    }

    private void nativeApplyGravityWrapper(float dt) {
        nativeApplyGravity(getWidth(), getHeight(), posX, posY, velX, velY, dt, gravity, radius);
    }

    private void nativeRestartGameWrapper() {
        nativeRestartGame(getWidth(), getHeight(), posX, posY, velX, velY);
    }

    private int nativeHandleTouchWrapper(float touchX, float touchY) {
        return nativeHandleTouch(touchX, touchY, posX, posY, radius);
    }

    private void nativeCheckEndWrapper() {
        nativeCheckEnd();
    }

    private int nativeGetHitCountWrapper() {
        return nativeGetHitCount();
    }

    private int nativeGetMissCountWrapper() {
        return nativeGetMissCount();
    }

    private boolean nativeGetPlayerWonWrapper() {
        return nativeGetPlayerWon();
    }

    private boolean nativeGetGameOverWrapper() {
        return nativeGetGameOver();
    }

    private native void nativeUpdate(
        float screenWidth,
        float screenHeight,
        float[] posX,
        float[] posY,
        float[] velX,
        float[] velY,
        float radius,
        float dt
    );

    private native void nativeRespawnBall(
        float screenWidth, 
        float screenHeight, 
        float[] posX,
        float[] posY,
        float[] velX,
        float[] velY,
        int index, 
        float radius
    );

    private native void nativeApplyGravity(
        float screenWidth, 
        float screenHeight, 
        float[] posX,
        float[] posY,
        float[] velX,
        float[] velY,
        float dt, 
        float gravity,
        float radius
    );

    private native void nativeRestartGame(
        float screenWidth, 
        float screenHeight, 
        float[] posX, 
        float[] posY, 
        float[] velX,
        float[] velY
    );

    private native int nativeHandleTouch(
        float touchX,
        float touchY,
        float[] posX,
        float[] posY,
        float radius
    );

    private native void nativeCheckEnd();

    private native int nativeGetHitCount();
    private native int nativeGetMissCount();

    private native boolean nativeGetPlayerWon();
    private native boolean nativeGetGameOver();

}