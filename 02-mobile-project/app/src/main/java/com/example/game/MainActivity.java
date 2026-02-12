package com.example.game;

import android.app.Activity;
import android.os.Bundle;
public class MainActivity extends Activity {
    private MenuView menuView; 
    private GameView gameView;

    @Override 
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        menuView = new MenuView(this, new MenuView.MenuListener() {
            @Override 
            public void onStartGame() {
                startGame();
            }
        });

        setContentView(menuView);
    }

    private void startGame() {
        gameView = new GameView(this);
        setContentView(gameView);
    }
}

