#pragma once
#include "raylib.h"

extern char* extractedFilePath;
extern char* previousExtractedFilePath;
extern Music mapAudio;
extern Texture2D mapBackground;

typedef struct Note {
    Vector2 position;
    int sliderVelocity;
    int timing;
    int isBlue;
    int bigNote;
    int isPressed;
    Color noteColor;
} Note;

typedef struct Beatmap {
    char* title;
    char* artist;
    char* difficultyName;
    char* audioFileName;
    int hpDrain;
    int od;
    char* backgroundFileName;
} Beatmap;