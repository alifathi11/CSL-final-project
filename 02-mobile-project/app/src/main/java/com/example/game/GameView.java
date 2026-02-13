package com.example.game;

import java.util.Random;

import android.content.Context;
import android.view.MotionEvent;
import android.view.View;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Color;

public class GameView extends View {

    private float gravity = 2000f;

    private boolean gameOver  = false;
    private boolean playerWon = false;

    private int winHitCount   = 10;
    private int loseMissCount = 10;

    private int hitCount;
    private int missCount;

    private int ballCount = 4;

    private float[] posX;
    private float[] posY;
    private float[] velX;
    private float[] velY;

    private boolean[] alive;
    
    private float radius = 100f;
    
    private Paint ballPaint; 
    private Paint textPaintWhite;
    private Paint textPaintRed;
    private Paint endPaint;

    private long lastTime;

    static {
        System.loadLibrary("game"); 
    }

    public GameView(Context context) {
        super(context);

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

        hitCount  = 0;
        missCount = 0;

        posX = new float[ballCount];
        posY = new float[ballCount];
        velX = new float[ballCount];
        velY = new float[ballCount];

        alive = new boolean[ballCount];

        Random rnd = new Random();

        for (int i = 0; i < ballCount; i++) {
            posX[i] = rnd.nextFloat() * 800;
            posY[i] = rnd.nextFloat() * 1200;
            velX[i] = (rnd.nextFloat() - 0.5f) * 2000;
            velY[i] = (rnd.nextFloat() - 0.5f) * 2000;

            alive[i] = true;
        }

        lastTime = System.nanoTime();
    }

    @Override 
    protected void onDraw(Canvas canvas) {

        long now = System.nanoTime();
        float dt = (now - lastTime) / 1_000_000_000f;
        if (dt > 0.033f) dt = 0.033f; 
        lastTime = now;
        
        update(dt);
        
        canvas.drawColor(Color.BLACK);

        for (int i = 0; i < ballCount; i++) {
            if (!alive[i]) continue;

            canvas.drawCircle(posX[i], posY[i], radius, ballPaint);
        
        }

        canvas.drawText("Hits: " + hitCount, 30f, 60f, textPaintWhite);
        canvas.drawText("Miss: " + missCount, 30f, 120f, textPaintRed);

        if (gameOver) {

            float cx = getWidth()  / 2f;
            float cy = getHeight() / 2f;

            endPaint.setColor(playerWon ? Color.GREEN : Color.RED);
            String msg = playerWon ? "You Win!" : "Game Over";
            canvas.drawText(msg, cx, cy, endPaint);
        }

        postInvalidateOnAnimation();
    }

    private void update(float dt) {

        if (gameOver) {
            nativeApplyGravityWrapper(dt);
            return;
        }

        nativeUpdateWrapper(dt);

        checkEnd();
    }

    @Override 
    public boolean onTouchEvent(MotionEvent event) {

        if (event.getAction() != MotionEvent.ACTION_DOWN)
            return true;

        if (gameOver) {
            restartGame();
            return true;
        }

        float touchX = event.getX();
        float touchY = event.getY();

        boolean hitSomething = false;

        float touchPadding = 75f;
        float touchRadius = radius + touchPadding;
        float touchRadiusSq = touchRadius * touchRadius;

        for (int i = 0; i < ballCount; i++) {

            if (!alive[i]) continue;

            float dx = touchX - posX[i];
            float dy = touchY - posY[i];

            if (dx * dx + dy * dy <= touchRadiusSq) {

                alive[i] = false;
                nativeRespawnBallWrapper(i);

                hitCount++;
                hitSomething = true;

                break;
            }
        }

        if (!hitSomething) {
            missCount++;
        }

        return true;
    }

    private void nativeUpdateWrapper(float dt) {
        if (getWidth() == 0 || getHeight() == 0) return;

        nativeUpdate(getWidth(), getHeight(), posX, posY, velX, velY, alive, radius, dt, gameOver);
    }

    private void nativeRespawnBallWrapper(int i) {
        nativeRespawnBall(getWidth(), getHeight(), posX, posY, velX, velY, alive, i, radius);
    }

    private void nativeApplyGravityWrapper(float dt) {
        nativeApplyGravity(getWidth(), getHeight(), posX, posY, velX, velY, alive, dt, gravity, radius);
    }

    private void restartGame() {
        hitCount = 0;
        missCount = 0;

        Random rnd = new Random();
        for (int i = 0; i < ballCount; i++) {
            posX[i] = rnd.nextFloat() * getWidth();
            posY[i] = rnd.nextFloat() * getHeight();
            velX[i] = (rnd.nextFloat() - 0.5f) * 4000;
            velY[i] = (rnd.nextFloat() - 0.5f) * 4000;
            alive[i] = true;
        }

        gameOver = false;
        playerWon = false;
    }

    private void checkEnd() {
        if (hitCount >= winHitCount) {
            gameOver  = true;
            playerWon = true;
        } else if (missCount >= loseMissCount) {
            gameOver  = true;
            playerWon = false;
        }
    }

    private native void nativeUpdate(
        float screenWidth,
        float screenHeight,
        float[] posX,
        float[] posY,
        float[] velX,
        float[] velY,
        boolean[] alive,
        float radius,
        float dt,
        boolean gameOver
    );

    private native void nativeRespawnBall(
        float screenWidth, 
        float screenHeight, 
        float[] posX,
        float[] posY,
        float[] velX,
        float[] velY,
        boolean[] alive,
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
        boolean[] alive,
        float dt, 
        float gravity,
        float radius
    );

}