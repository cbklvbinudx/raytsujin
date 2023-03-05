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