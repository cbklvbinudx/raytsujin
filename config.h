#pragma once

extern int gameStateSwitch;
extern int lastGameState;
extern const int screenWidth;
extern const int screenHeight;
extern const char* songsDirectory;
extern char* pathToDifficulty;

enum GameState {
    Menu,
    Playing,
    Paused,
    Failed,
    Finished,
};
