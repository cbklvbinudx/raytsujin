#include "raylib.h"
#include "beatmap.h"
#include "mainMenu.h"
#include "config.h"
#include <math.h>
#include <stdlib.h>
#include "finishScreen.h"

void DrawElementsPlaying();
void DrawPlayfield();
void DrawNote(Note* taikoNote);
void UpdateGamePlaying();
void RetryButton();

enum Judgements {
    Great,
    Good,
    Miss,
    Unhit,
};

float songTimeElapsed;

int wasPressedLastFrame = 0;
int lastNoteTiming = 0; // Used for judgement feedback images (hit, miss)
int currentNote = 0;
float deltaPress = 0;

int comboCounter = 0;
int missCounter = 0;
int goodCounter = 0;
int greatCounter = 0;

Texture2D mapBackground;
Texture2D taikoMiss;
Texture2D taikoGood;
Texture2D taikoGreat;

float scrollFieldHeight = 100.0f; // Support for numbers different than 100 doesn't really work yet
const float scrollFieldOffset = 50.0f;

const float hitWindow = 100.0f;

const int screenWidth = 1600;
const int screenHeight = 900;

Sound redSound;
Sound blueSound;
Sound comboBreak;
Music audio;

Image icon;

int gameStateSwitch = Menu; // Starting in the menu
int lastGameState = 0;
int judgementSwitch = Unhit;

int main() {

    InitAudioDevice();

    InitWindow(screenWidth, screenHeight, "raytsujin");
    icon = LoadImage("resources/teri.png"); // TODO: Change this icon to something more fitting
    SetWindowIcon(icon);

    SetExitKey(KEY_NULL);

    taikoMiss = LoadTexture("resources/taiko-hit0.png");
    taikoGood = LoadTexture("resources/taiko-hit100.png");
    taikoGreat = LoadTexture("resources/taiko-hit300.png");

    redSound = LoadSound("resources/drum-hitfinish.wav");
    blueSound = LoadSound("resources/drum-hitclap.wav");
    comboBreak = LoadSound("resources/combobreak.wav");

    while(!WindowShouldClose()) {
        if(gameStateSwitch == Menu) {
            DrawMainMenu();
            UpdateMainMenu();
            lastGameState = Menu;
        }
        else if(gameStateSwitch == Playing) {
            if(lastGameState != Playing) {
                PlayMusicStream(audio);
            }
            if(GetMusicTimePlayed(audio) > GetMusicTimeLength(audio) - 0.01) {
                gameStateSwitch = Finished;
            }
            songTimeElapsed = GetMusicTimePlayed(audio) * 1000; // Time played in milliseconds, for use with .osu file timing
            lastGameState = Playing;
            UpdateMusicStream(audio);
            UpdateGamePlaying();
            DrawElementsPlaying();
        } else if(gameStateSwitch == Finished) {
            DrawFinishScreen();
            UpdateFinishScreen();
            if(lastGameState != Finished) {
                StopMusicStream(audio);
            }
            lastGameState = Finished;
        }
    }

    CloseAudioDevice();

    UnloadTexture(mapBackground);
    UnloadTexture(taikoGreat);
    UnloadTexture(taikoMiss);
    UnloadImage(icon);

    UnloadSound(redSound);
    UnloadSound(blueSound);
    UnloadSound(comboBreak);
    UnloadMusicStream(audio);

    FreeBeatmap(currentBeatmap);
    free(extractedFilePath);
    free(previousExtractedFilePath);

    CloseWindow();

    return 0;
}

void DrawElementsPlaying() {
    BeginDrawing();
    
    ClearBackground(RAYWHITE);
    DrawPlayfield();

    for (int i = 0; i < currentBeatmap->noteCount; i++)
    {
        DrawNote(currentBeatmap->notes + i);

        currentBeatmap->notes[i].position.x = scrollFieldHeight + currentBeatmap->notes[i].timing - songTimeElapsed; // Offset by the diameter of the destination circle
    }    

    if(judgementSwitch == Miss && songTimeElapsed - lastNoteTiming < 300) {
        DrawTexture(taikoMiss, -70, -50 + scrollFieldOffset, WHITE); // TODO: Animate this (fade in fade out or scale)
    }
    if(judgementSwitch == Great && songTimeElapsed - lastNoteTiming < 300) {
        DrawTexture(taikoGreat, -90, -40 + scrollFieldOffset, WHITE); // TODO: Animate this (fade in fade out or scale)
    }
    if(judgementSwitch == Good && songTimeElapsed - lastNoteTiming < 300) {
        DrawTexture(taikoGood, -90, -40 + scrollFieldOffset, WHITE); // TODO: Animate this (fade in fade out or scale)
    }


    DrawFPS(2, 0);
    DrawText(TextFormat("Misses: %i", missCounter), 2, screenHeight - 100, 16, WHITE);
    DrawText(TextFormat("Good: %i", goodCounter), 2, screenHeight - 80, 16, WHITE);
    DrawText(TextFormat("Great: %i", greatCounter), 2, screenHeight - 60, 16, WHITE);
    DrawText(TextFormat("%ix", comboCounter), 2, screenHeight - 40, 48, WHITE);

    EndDrawing();
}

void DrawPlayfield() {
    DrawTexturePro(mapBackground, (Rectangle) { 0, 0, screenWidth, screenHeight },
                   (Rectangle) { 0, 0, screenWidth, screenHeight }, (Vector2) { 0, 0 }, 0,
                   WHITE);
    DrawRectangleGradientH(0, scrollFieldOffset, screenWidth, scrollFieldHeight, Fade(GRAY, 0.8f), Fade(BLACK, 0.8f));
    DrawCircleGradient(0, screenHeight, 150, BLACK, BLANK);
    DrawCircle(50, scrollFieldHeight / 2 + scrollFieldOffset, scrollFieldHeight / 2, BLACK); // The destination circle
}

void DrawNote(Note* taikoNote) {
    if(taikoNote->bigNote) {
        DrawCircleV(taikoNote->position, scrollFieldHeight / 2 + 15, taikoNote->noteColor);
        DrawCircleLines(taikoNote->position.x, taikoNote->position.y, scrollFieldHeight / 2 + 15, BLACK);

    } else {
        DrawCircleV(taikoNote->position, scrollFieldHeight / 2, taikoNote->noteColor);
        DrawCircleLines(taikoNote->position.x, taikoNote->position.y, scrollFieldHeight / 2, BLACK);
    }
}

void UpdateGamePlaying() {
    int timingProper = currentBeatmap->notes[currentNote].timing + scrollFieldHeight; // Offset by the diameter of the destination circle

    deltaPress = fabsf(timingProper - songTimeElapsed);

    if((timingProper < songTimeElapsed - hitWindow)) {
        // If no note pressed in time
        comboCounter = 0;
        PlaySound(comboBreak);
        currentNote++;
        missCounter++;
        judgementSwitch = Miss;
    }
    // Blue note logic
    else if(currentBeatmap->notes[currentNote].isBlue) {
        if((IsKeyPressed(KEY_D) || IsKeyPressed(KEY_K)) && (deltaPress < hitWindow) && currentBeatmap->notes[currentNote].isPressed == 0 && !wasPressedLastFrame) {
            // If note is hit
            currentBeatmap->notes[currentNote].isPressed = 1;
            currentBeatmap->notes[currentNote].noteColor = Fade(BLUE, 0.4f);

            lastNoteTiming = timingProper;
            if(deltaPress < hitWindow - 33) {
                judgementSwitch = Great;
                greatCounter++;
            } else {
                judgementSwitch = Good;
                goodCounter++;
            }
            comboCounter++;
            wasPressedLastFrame = 1;
            currentNote++;
        } else if((IsKeyPressed(KEY_F) || IsKeyPressed(KEY_J)) && (deltaPress < hitWindow) && currentBeatmap->notes[currentNote].isPressed == 0 && !wasPressedLastFrame) {
            // If wrong note is pressed

            PlaySound(comboBreak);

            currentBeatmap->notes[currentNote].isPressed = 1;
            currentBeatmap->notes[currentNote].noteColor = BLANK;

            lastNoteTiming = timingProper;
            judgementSwitch = Miss;
            comboCounter = 0;
            wasPressedLastFrame = 1;
            currentNote++;
            missCounter++;
        }
    // Red note logic
    } else {
        if((IsKeyPressed(KEY_F) || IsKeyPressed(KEY_J)) && (deltaPress < hitWindow) && currentBeatmap->notes[currentNote].isPressed == 0 && !wasPressedLastFrame) {
            // If note is hit
            currentBeatmap->notes[currentNote].isPressed = 1;
            currentBeatmap->notes[currentNote].noteColor = Fade(RED, 0.4f);

            lastNoteTiming = timingProper;
            if(deltaPress < hitWindow - 33) {
                judgementSwitch = Great;
                greatCounter++;
            } else {
                judgementSwitch = Good;
                goodCounter++;
            }
            comboCounter++;
            wasPressedLastFrame = 1;
            currentNote++;
        } else if((IsKeyPressed(KEY_D) || IsKeyPressed(KEY_K)) && (deltaPress < hitWindow) && currentBeatmap->notes[currentNote].isPressed == 0 && !wasPressedLastFrame) {
            // If wrong note is pressed

            PlaySound(comboBreak);

            currentBeatmap->notes[currentNote].isPressed = 1;
            currentBeatmap->notes[currentNote].noteColor = BLANK;

            lastNoteTiming = timingProper;
            judgementSwitch = Miss;
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
    if(IsKeyPressed(KEY_GRAVE)) {
        RetryButton();
    }
    if(IsKeyPressed(KEY_ESCAPE) && gameStateSwitch == Playing) {
        StopMusicStream(audio);
        gameStateSwitch = Menu;
        lastGameState = Menu;

        ResetGameplayVariables();
    }
}

void RetryButton() {
    if(IsMusicStreamPlaying(audio)) {
        StopMusicStream(audio);
        PlayMusicStream(audio);

        ResetGameplayVariables();
    }
}

void ResetGameplayVariables() {

    for(int i = 0; i < currentBeatmap->noteCount; i++) {
        currentBeatmap->notes[i].isPressed = 0;
        currentBeatmap->notes[i].noteColor = currentBeatmap->notes[i].isBlue?BLUE:RED;
    }

    comboCounter = 0;
    missCounter = 0;
    goodCounter = 0;
    greatCounter = 0;
    judgementSwitch = Unhit;
    lastNoteTiming = 0;
    currentNote = 0;
}