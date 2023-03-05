#include "raylib.h"
#include "osuProcessing.c"

/* NOTE: first read the .osu file and add Notes into an array */

void DrawElements();
void DrawPlayfield();
void SendNote(Note taikoNote);
void UpdateGame();

int frameCounter;

int wasPressedLastFrame = 0;
int lastNoteTiming = 0; // Used for hit feedback (miss, hit)
int isMiss = 0;
int isHit = 0; // TODO: 100s and 300s
int comboCounter = 0;

Texture2D akari;
Texture2D taikoMiss;
Texture2D taikoHit;

int arbitraryNumber = 100; // Temporary

const int hitWindow = 20;

const int screenWidth = 800;
const int screenHeight = 480;

Sound redSound;
Sound blueSound;
Sound comboBreak;
Sound mapAudio;

Color redColor;
Color blueColor;

Image icon;

int main() {

    icon = LoadImage("resources/teri.png");
    InitAudioDevice();

    SetWindowIcon(icon); // Doesn't work
    InitWindow(screenWidth, screenHeight, "Taiko");
    SetTargetFPS(1000);

    // Random values until .osu file processing is done
    // for(int i = 0; i < MAX_NOTES; i++)
    // {
    //     arbitraryNumber += GetRandomValue(40, 70);
    //     Notes[i].position = notePosition;
    //     Notes[i].isPressed = 0;
    //     Notes[i].timing = arbitraryNumber;
    //     Notes[i].sliderVelocity = GetRandomValue(4, 10);
    //     Notes[i].isBlue = GetRandomValue(0, 1);
    //     Notes[i].noteColor = Notes[i].isBlue?BLUE:RED;
    // }

    akari = LoadTexture("resources/akari.png");
    taikoMiss = LoadTexture("resources/taiko-hit0.png");
    taikoHit = LoadTexture("resources/taiko-hit300.png");

    redSound = LoadSound("resources/drum-hitfinish.wav");
    blueSound = LoadSound("resources/drum-hitclap.wav");
    comboBreak = LoadSound("resources/combobreak.wav");

    mapAudio = LoadSound("resources/Songs/1600415 jung jaeil - Way Back then/audio.mp3"); // Temporary

    PlaySound(mapAudio);

    StartOsuFileProcessing("resources/Songs/1600415 jung jaeil - Way Back then/jung jaeil - Way Back then (tadahitotsu) [456].osu");

    while(!WindowShouldClose()) {
        frameCounter++;
        UpdateGame();
        DrawElements();
    }

    CloseAudioDevice();

    UnloadTexture(akari);
    UnloadTexture(taikoHit);
    UnloadTexture(taikoMiss);

    CloseWindow();

    return 0;
}

void DrawElements() {
    BeginDrawing();
    
    ClearBackground(RAYWHITE);
    DrawPlayfield();

    for (int i = 0; i < MAX_NOTES; i++)
    {
        SendNote(Notes[i]);
        if(frameCounter >= Notes[i].timing) {
            Notes[i].position.x -= Notes[i].sliderVelocity;
        }
    }    

    if(isMiss && !(frameCounter - lastNoteTiming >= 10 )) {
        DrawTexture(taikoMiss, -70, -50, WHITE);
    }
    if(isHit && !(frameCounter - lastNoteTiming >= 10 )) {
        DrawTexture(taikoHit, -90, -40, WHITE);
    }

    DrawFPS(2, 0);
    DrawText(TextFormat("frames: %i", frameCounter), 100, 0, 16, LIME);
    DrawText(TextFormat("%ix", comboCounter), 2, 440, 48, BLACK);

    EndDrawing();
}

void DrawPlayfield() {
    DrawRectangleGradientH(0, 0, 800, 100, LIGHTGRAY, BLACK);
    DrawCircle(50, 50, 50, BLACK);

    DrawTexture(akari, 300, 200, WHITE);
}

void SendNote(Note taikoNote) {
    if(frameCounter >= taikoNote.timing) {
        if(taikoNote.isBlue) {
            DrawCircleV(taikoNote.position, 50, taikoNote.noteColor);
        } else {
            DrawCircleV(taikoNote.position, 50, taikoNote.noteColor);
        }
    }
}

void UpdateGame() {
    for (int i = 0; i < noteCounter; i++)
    {
        // The actual hit window, most probably Disaster and Doesn't Work.
        int timingProper = Notes[i].timing + screenWidth / Notes[i].sliderVelocity;

        // Blue note logic
        if(Notes[i].isBlue) {
            if((IsKeyPressed(KEY_D) || IsKeyPressed(KEY_K)) && ((frameCounter >= timingProper && frameCounter <= timingProper + hitWindow)
                                        || (frameCounter <= timingProper && frameCounter >= timingProper - hitWindow)) && Notes[i].isPressed == 0 && !wasPressedLastFrame) {
                Notes[i].isPressed = 1;
                Notes[i].noteColor = Fade(BLUE, 0.1f);

                lastNoteTiming = timingProper;
                isMiss = 0;
                isHit = 1;
                comboCounter++;
                wasPressedLastFrame = 1;
            } else if((IsKeyPressed(KEY_F) || IsKeyPressed(KEY_J)) && ((frameCounter >= timingProper && frameCounter <= timingProper + hitWindow)
                                        || (frameCounter <= timingProper && frameCounter >= timingProper - hitWindow)) && Notes[i].isPressed == 0 && !wasPressedLastFrame) {
                
                PlaySound(comboBreak);

                Notes[i].isPressed = 1;
                Notes[i].noteColor = BLANK;

                lastNoteTiming = timingProper;
                isMiss = 1;
                isHit = 0;
                comboCounter = 0;
                wasPressedLastFrame = 1;
            }
        // Red note logic
        } else {
            if((IsKeyPressed(KEY_F) || IsKeyPressed(KEY_J)) && ((frameCounter >= timingProper && frameCounter <= timingProper + hitWindow)
                                        || (frameCounter <= timingProper && frameCounter >= timingProper - hitWindow)) && Notes[i].isPressed == 0 && !wasPressedLastFrame) {
                Notes[i].isPressed = 1;
                Notes[i].noteColor = Fade(RED, 0.1f);

                lastNoteTiming = timingProper;
                isMiss = 0;
                isHit = 1;
                comboCounter++;
                wasPressedLastFrame = 1;
            } else if((IsKeyPressed(KEY_D) || IsKeyPressed(KEY_K)) && ((frameCounter >= timingProper && frameCounter <= timingProper + hitWindow)
                                        || (frameCounter <= timingProper && frameCounter >= timingProper - hitWindow)) && Notes[i].isPressed == 0 && !wasPressedLastFrame) {
                
                PlaySound(comboBreak);

                Notes[i].isPressed = 1;
                Notes[i].noteColor = BLANK;

                lastNoteTiming = timingProper;
                isMiss = 1;
                isHit = 0;
                comboCounter = 0;
                wasPressedLastFrame = 1;
            }
        }
    }
    if(GetKeyPressed() == 0) {
        wasPressedLastFrame = 0;
    }
    if(IsKeyDown(KEY_D) || IsKeyDown(KEY_K)) {
        DrawText("BLUE PRESSED", 150, 200, 16, BLUE);
    }
    if(IsKeyDown(KEY_F) || IsKeyDown(KEY_J)) {
        DrawText("RED PRESSED", 150, 220, 16, RED);
    }
    if(IsKeyPressed(KEY_D) || IsKeyPressed(KEY_K)) {
        PlaySound(blueSound);
    }
    if(IsKeyPressed(KEY_F) || IsKeyPressed(KEY_J)) {
        PlaySound(redSound);
    }
}