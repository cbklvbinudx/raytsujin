#pragma once
#include "raylib.h"

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

extern char* extractedFilePath;
extern char* previousExtractedFilePath;
extern Music mapAudio;

extern Texture2D mapBackground;
extern int noteCount;
extern Note* notes;
extern Beatmap* currentBeatmap;

Beatmap* LoadBeatmapFromFile(char* filename);
void FreeBeatmap(Beatmap* beatmap);

int GetBeatmapInfoInt(char* line);
char* GetBeatmapInfoString(char* line);
