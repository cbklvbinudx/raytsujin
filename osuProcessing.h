#pragma once
#include "beatmapInfo.h"
#include "raylib.h"

extern int noteCount;
extern Note* notes;
extern Beatmap* currentBeatmap;

Beatmap* LoadBeatmapFromFile(char* filename);
void FreeBeatmap(Beatmap* beatmap);

int GetBeatmapInfoInt(char* line);
char* GetBeatmapInfoString(char* line);
