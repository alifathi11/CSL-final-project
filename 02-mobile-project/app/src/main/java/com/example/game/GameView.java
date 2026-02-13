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

    private long lastTime;

    private float currentSpeedMultiplier = 1.0f;
    private float maxSpeedMultiplier = 2.0f;
    private float speedIncreaseStep = 0.1f;

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
            Paint endPaint = new Paint();
            endPaint.setColor(playerWon ? Color.GREEN : Color.RED);
            endPaint.setTextSize(120f);
            endPaint.setTextAlign(Paint.Align.CENTER);
            endPaint.setAntiAlias(true);

            float cx = getWidth()  / 2f;
            float cy = getHeight() / 2f;

            String msg = playerWon ? "You Win!" : "Game Over";
            canvas.drawText(msg, cx, cy, endPaint);
        }

        postInvalidateOnAnimation();
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
                respawnBall(i);

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

    private void update(float dt) {

        if (gameOver) {
            applyGravity(dt);
            return;
        }

        nativeUpdateWrapper(dt);

        checkEnd();
    }

    private void respawnBall(int i) {

        if (currentSpeedMultiplier < maxSpeedMultiplier) {
            currentSpeedMultiplier += speedIncreaseStep;
            speedIncreaseStep *= 0.8;
        }

        Random rnd = new Random();

        int side = rnd.nextInt(4);

        float w = getWidth();
        float h = getHeight();

        float baseSpeedX = (rnd.nextFloat() * 2000f) + 1000f;
        float baseSpeedY = (rnd.nextFloat() * 2000f) + 1000f;

        float speedX = baseSpeedX * currentSpeedMultiplier;
        float speedY = baseSpeedY * currentSpeedMultiplier;

        switch (side) {

            case 0:
                posX[i] = rnd.nextFloat() * w;
                posY[i] = -radius;
                velX[i] = (rnd.nextFloat() - 0.5f) * speedX;
                velY[i] = Math.abs(speedY);
                break;

            case 1:
                posX[i] = rnd.nextFloat() * w;
                posY[i] = h + radius;
                velX[i] = (rnd.nextFloat() - 0.5f) * speedX;
                velY[i] = -Math.abs(speedY);
                break;

            case 2:
                posX[i] = -radius;
                posY[i] = rnd.nextFloat() * h;
                velX[i] = Math.abs(speedX);
                velY[i] = (rnd.nextFloat() - 0.5f) * speedY;
                break;

            case 3:
                posX[i] = w + radius;
                posY[i] = rnd.nextFloat() * h;
                velX[i] = -Math.abs(speedX);
                velY[i] = (rnd.nextFloat() - 0.5f) * speedY;
                break;
        }

        alive[i] = true;
    }

    private void restartGame() {
        hitCount = 0;
        missCount = 0;
        currentSpeedMultiplier = 1.0f;

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

    private void applyGravity(float dt) {

        for (int i = 0; i < ballCount; i++) {
            if (!alive[i]) continue;

            velY[i] += gravity * dt;       
            posY[i] += velY[i] * dt;

            if (posY[i] > getHeight() - radius) {
                posY[i] = getHeight() - radius;
                velY[i] *= -0.3f;  
            }
        }

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
        int screenWidth,
        int screenHeight,
        float[] posX,
        float[] posY,
        float[] velX,
        float[] velY,
        boolean[] alive,
        float radius,
        float dt,
        boolean gameOver
    );

}