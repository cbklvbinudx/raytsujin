#include "raylib.h"
#include "osuProcessing.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

void DrawElements();
void DrawPlayfield();
void SendNote(Note taikoNote);
void UpdateGame();

float songTimeElapsed;

int wasPressedLastFrame = 0;
int lastNoteTiming = 0; // Used for hit feedback images (miss, hit)
int isMiss = 0;
int isHit = 0; // TODO: 100s and 300s
int currentNote = 0;
float deltaPress = 0;

int comboCounter = 0;
int hitCounter = 0;
int missCounter = 0;

Texture2D akari; // TODO: Change this to map background
Texture2D taikoMiss;
Texture2D taikoHit;

const int hitWindow = 50;

const int screenWidth = 800;
const int screenHeight = 480;

Sound redSound;
Sound blueSound;
Sound comboBreak;
Music mapAudio;

Image icon;

int main() {

    InitAudioDevice();

    InitWindow(screenWidth, screenHeight, "Taiko");
    icon = LoadImage("resources/teri.png"); // TODO: Change this icon to something more fitting
    SetWindowIcon(icon);

    taikoMiss = LoadTexture("resources/taiko-hit0.png");
    taikoHit = LoadTexture("resources/taiko-hit300.png");

    redSound = LoadSound("resources/drum-hitfinish.wav");
    blueSound = LoadSound("resources/drum-hitclap.wav");
    comboBreak = LoadSound("resources/combobreak.wav");

    mapAudio = LoadMusicStream("resources/Songs/1600415 jung jaeil - Way Back then/audio.mp3"); // Temporary

    PlayMusicStream(mapAudio);

    StartOsuFileProcessing("resources/Songs/1600415 jung jaeil - Way Back then/jung jaeil - Way Back then (tadahitotsu) [456].osu");

    char* stringBuffer = malloc(strlen("resources/Songs/1600415 jung jaeil - Way Back then/") + strlen(beatmap.backgroundFileName) + 1);
    strcpy(stringBuffer, "resources/Songs/1600415 jung jaeil - Way Back then/");
    strcat(stringBuffer, beatmap.backgroundFileName);
    akari = LoadTexture(stringBuffer);

    while(!WindowShouldClose()) {
        songTimeElapsed = GetMusicTimePlayed(mapAudio) * 1000;
        UpdateMusicStream(mapAudio);
        UpdateGame();
        DrawElements();
    }

    CloseAudioDevice();

    UnloadTexture(akari);
    UnloadTexture(taikoHit);
    UnloadTexture(taikoMiss);

    free(beatmap.audioFileName);
    free(beatmap.backgroundFileName);
    free(stringBuffer);

    CloseWindow();

    return 0;
}

void DrawElements() {
    BeginDrawing();
    
    ClearBackground(RAYWHITE);
    DrawPlayfield();

    for (int i = 0; i < noteCounter; i++)
    {
        SendNote(Notes[i]);

        Notes[i].position.x = 100 + Notes[i].timing - songTimeElapsed;
    }    

    if(isMiss && songTimeElapsed - lastNoteTiming < 300) {
        DrawTexture(taikoMiss, -70, -50, WHITE); // TODO: Animate this (fade in fade out or scale)
    }
    if(isHit && songTimeElapsed - lastNoteTiming < 300) {
        DrawTexture(taikoHit, -90, -40, WHITE); // TODO: Animate this (fade in fade out or scale)
    }

    DrawFPS(2, 0);
    DrawText(TextFormat("Misses: %i", missCounter), 2, 420, 16, BLACK);
    DrawText(TextFormat("Hits: %i", hitCounter), 2, 400, 16, BLACK);
    DrawText(TextFormat("%ix", comboCounter), 2, 440, 48, BLACK);

    EndDrawing();
}

void DrawPlayfield() {
    DrawRectangleGradientH(0, 0, 800, 100, LIGHTGRAY, BLACK);
    DrawCircle(50, 50, 50, BLACK);

    DrawTexture(akari, 300, 200, WHITE);
}

void SendNote(Note taikoNote) {
    DrawCircleV(taikoNote.position, 50, taikoNote.noteColor);
}

void UpdateGame() {
    int timingProper = Notes[currentNote].timing + 100;

    deltaPress = fabsf(timingProper - songTimeElapsed);

    if((timingProper < songTimeElapsed - hitWindow)) {
        // If no note pressed in time
        comboCounter = 0;
        PlaySound(comboBreak);
        currentNote++;
        missCounter++;
    }
    // Blue note logic
    else if(Notes[currentNote].isBlue) {
        if((IsKeyPressed(KEY_D) || IsKeyPressed(KEY_K)) && (deltaPress < hitWindow) && Notes[currentNote].isPressed == 0 && !wasPressedLastFrame) {
            // If note is hit
            Notes[currentNote].isPressed = 1;
            Notes[currentNote].noteColor = Fade(BLUE, 0.1f);

            lastNoteTiming = timingProper;
            isMiss = 0;
            isHit = 1;
            comboCounter++;
            wasPressedLastFrame = 1;
            currentNote++;
            hitCounter++;
        } else if((IsKeyPressed(KEY_F) || IsKeyPressed(KEY_J)) && (deltaPress < hitWindow) && Notes[currentNote].isPressed == 0 && !wasPressedLastFrame) {
            // If wrong note is pressed

            PlaySound(comboBreak);

            Notes[currentNote].isPressed = 1;
            Notes[currentNote].noteColor = BLANK;

            lastNoteTiming = timingProper;
            isMiss = 1;
            isHit = 0;
            comboCounter = 0;
            wasPressedLastFrame = 1;
            currentNote++;
            missCounter++;
        }
    // Red note logic
    } else {
        if((IsKeyPressed(KEY_F) || IsKeyPressed(KEY_J)) && (deltaPress < hitWindow) && Notes[currentNote].isPressed == 0 && !wasPressedLastFrame) {
            // If note is hit
            Notes[currentNote].isPressed = 1;
            Notes[currentNote].noteColor = Fade(RED, 0.1f);

            lastNoteTiming = timingProper;
            isMiss = 0;
            isHit = 1;
            comboCounter++;
            wasPressedLastFrame = 1;
            currentNote++;
            hitCounter++;
        } else if((IsKeyPressed(KEY_D) || IsKeyPressed(KEY_K)) && (deltaPress < hitWindow) && Notes[currentNote].isPressed == 0 && !wasPressedLastFrame) {
            // If wrong note is pressed

            PlaySound(comboBreak);

            Notes[currentNote].isPressed = 1;
            Notes[currentNote].noteColor = BLANK;

            lastNoteTiming = timingProper;
            isMiss = 1;
            isHit = 0;
            comboCounter = 0;
            wasPressedLastFrame = 1;
            currentNote++;
            missCounter++;
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