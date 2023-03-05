#pragma once
#include "note.h"
#include "raylib.h"

extern int noteCounter;
extern Note Notes[512];
extern int hitObjectsSwitch;
extern Vector2 notePosition;

void StartOsuFileProcessing(char* filename);