package com.example.game;

import android.app.Activity;
import android.os.Bundle;
import android.content.Context;
import android.view.View;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Color;

public class MainActivity extends Activity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(new GameView(this));
    }
}

class GameView extends View {
    private Paint paint;

    public GameView(Context context) {
        super(context);
        paint = new Paint();
        paint.setColor(Color.RED);
        paint.setTextSize(80f);
        paint.setTextAlign(Paint.Align.CENTER);
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);
        float x = getWidth() / 2f;
        float y = getHeight() / 2f;
        canvas.drawText("Hello World", x, y, paint);
    }
}

