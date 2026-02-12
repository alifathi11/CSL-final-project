package com.example.game;

import android.content.Context;
import android.view.MotionEvent;
import android.view.View;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Color;

public class GameView extends View {
    
    private Paint paint; 
    private float ballX = 300;
    private float ballY = 300;

    public GameView(Context context) {
        super(context);

        paint = new Paint();
        paint.setColor(Color.GREEN);
    }

    @Override 
    protected void onDraw(Canvas canvas) {
        canvas.drawColor(Color.DKGRAY);
        canvas.drawCircle(ballX, ballY, 100, paint);
    }

    @Override 
    public boolean onTouchEvent(MotionEvent event) {
        if (event.getAction() == MotionEvent.ACTION_MOVE) {
            ballX = event.getX();
            ballY = event.getY();
            invalidate();
            return true;
        }
        
        return true;
    }
}