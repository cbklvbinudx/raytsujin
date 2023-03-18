#pragma once

extern int gameStateSwitch;
extern int lastGameState;
extern const float scrollFieldOffset;
extern int screenWidth;
extern int screenHeight;

enum GameState {
    Menu,
    Playing,
    Failed,
    Finished,
};