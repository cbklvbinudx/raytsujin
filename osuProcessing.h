#pragma once
#include "beatmapInfo.h"
#include "raylib.h"

extern int noteCounter;
extern Note Notes[512];
extern Beatmap beatmap;

void StartOsuFileProcessing(char* filename);

int GetBeatmapInfoInt(char* line);
char* GetBeatmapInfoString(char* line);
