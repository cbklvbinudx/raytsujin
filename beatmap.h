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
    Note* notes;
    int noteCount;
    int hpDrain;
    int od;
    char* title;
    char* artist;
    char* difficultyName;
    char* audioFileName;
    char* backgroundFileName;
    char* directory;
    Texture2D background;
    Music audio;
} Beatmap;

extern Beatmap* currentBeatmap;
extern float hitWindowGreat;
extern float hitWindowGood;
extern float hitWindowMiss;

Beatmap* LoadBeatmapFromFile(const char* filename);
void FreeBeatmap(Beatmap* beatmap);

void LoadBeatmapAudio(Beatmap* beatmap);
void LoadBeatmapBackground(Beatmap* beatmap);

int GetBeatmapInfoInt(char* line);
char* GetBeatmapInfoString(char* line);

void ResetGameplayVariables();
