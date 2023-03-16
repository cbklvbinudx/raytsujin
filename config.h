#pragma once

extern int gameStateSwitch;
extern int lastGameState;
extern const float scrollFieldOffset;
extern const int screenWidth;
extern const int screenHeight;

enum GameState {
    Menu,
    Playing,
    Failed,
    Finished,
};

void ResetGameplayVariables();
