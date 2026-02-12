package com.example.game;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.view.MotionEvent;
import android.view.View;

public class MenuView extends View {

    public interface MenuListener {
        void onStartGame();
    }

    private MenuListener listener; 
    private Paint paint; 

    public MenuView(Context context, MenuListener listener) {
        super(context);
        this.listener = listener; 

        paint = new Paint();
        paint.setColor(Color.WHITE);
        paint.setTextSize(80);
        paint.setTextAlign(Paint.Align.CENTER);
    }

    @Override 
    protected void onDraw(Canvas canvas) {
        canvas.drawColor(Color.BLACK);
        canvas.drawText("Tap to Start", getWidth()/2, getHeight()/2, paint);
    }

    @Override 
    public boolean onTouchEvent(MotionEvent event) {
        if (event.getAction() == MotionEvent.ACTION_DOWN) {
            listener.onStartGame();
            return true;
        }

        return false;
    }

}