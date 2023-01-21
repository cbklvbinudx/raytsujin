#include "raylib.h"

#define MAX_NOTES 100 // Temporary until .osu processing is done

typedef struct Note {
    Vector2 position;
    int sliderVelocity;
    int timing;
    int isBlue;
    int bigNote;
    int isPressed;
    Color noteColor;
} Note;