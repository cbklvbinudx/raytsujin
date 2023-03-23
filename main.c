#include "raylib.h"
#include "beatmap.h"
#include "mainMenu.h"
#include "config.h"
#include <math.h>
#include <stdio.h>
#include "finishScreen.h"
#include "raygui.h"

void DrawElementsPlaying();
void DrawPlayfield();
void DrawNote(Note* taikoNote);
void UpdateGamePlaying();
void RetryButton();
void NoteHit(int judgement);
void NoteMiss();

enum Judgements {
    Great,
    Good,
    Miss,
    Unhit,
};

float songTimeElapsed;

float lastNoteTiming = 0.0f; // Used for judgement feedback images (hit, miss)
int currentNote = 0;

int comboCounter = 0;
int missCounter = 0;
int goodCounter = 0;
int greatCounter = 0;

Texture2D taikoMiss;
Texture2D taikoGood;
Texture2D taikoGreat;

const float scrollFieldHeight = 100.0f;
const float destinationCircleOffset = 100.0f;
const int scrollFieldOffset = 50;

const Vector2 destinationCirclePosition = { destinationCircleOffset, scrollFieldHeight / 2 + (float)scrollFieldOffset};

int screenWidth = 1600;
int screenHeight = 900;

float currentVolume = 1.0f;
float accuracy = 1.0f;

Sound redSound;
Sound blueSound;
Sound comboBreak;

Image icon;

int gameStateSwitch = Menu; // Starting in the menu
int lastGameState = 0;
int judgementSwitch = Unhit;

int main() {

    InitAudioDevice();

    InitWindow(screenWidth, screenHeight, "raytsujin");
    icon = LoadImage("resources/teri.png"); // TODO: Change this icon to something more fitting
    SetWindowIcon(icon);

    GuiLoadStyle("resources/dark.rgs");

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
                PlayMusicStream(currentBeatmap->audio);
            }
            UpdateMusicStream(currentBeatmap->audio);
            songTimeElapsed = GetMusicTimePlayed(currentBeatmap->audio) * 1000.f; // Time played in milliseconds, for use with .osu file timing
            if(songTimeElapsed > GetMusicTimeLength(currentBeatmap->audio) * 1000.f - 0.01f) {
                gameStateSwitch = Finished;
            }
            lastGameState = Playing;
            UpdateGamePlaying();
            DrawElementsPlaying();
        } else if(gameStateSwitch == Finished) {
            DrawFinishScreen();
            UpdateFinishScreen();
            if(lastGameState != Finished) {
                StopMusicStream(currentBeatmap->audio);
            }
            lastGameState = Finished;
        }

        if(IsKeyPressed(KEY_UP) && currentVolume < 1.0f) {
            currentVolume += 0.05f;
            SetMasterVolume(currentVolume);
        } else if(IsKeyPressed(KEY_DOWN)) {
            currentVolume -= 0.05f;
            SetMasterVolume(currentVolume);
        }
    }

    CloseAudioDevice();

    UnloadTexture(taikoGreat);
    UnloadTexture(taikoMiss);
    UnloadImage(icon);

    UnloadSound(redSound);
    UnloadSound(blueSound);
    UnloadSound(comboBreak);

    if(currentBeatmap)
        FreeBeatmap(currentBeatmap);

    CloseWindow();

    return 0;
}

void DrawElementsPlaying() {
    BeginDrawing();
    
    ClearBackground(RAYWHITE);
    DrawPlayfield();

    if(currentBeatmap->notes[0].timing - songTimeElapsed > 1500) {
        DrawRectangleGradientV(screenWidth / 2 - 450 / 2, screenHeight - 100, 450, 100, BLANK, Fade(BLACK, 0.6f));
        DrawTextEx(GuiGetFont(), "PRESS SPACE TO SKIP",
                   (Vector2) { (float)screenWidth / 2 - MeasureTextEx(GuiGetFont(), "PRESS SPACE TO SKIP",48, 2).x / 2,
                               (float)screenHeight - 65 }, 48, 2, WHITE);
    }

    if(IsKeyDown(KEY_K)) {
        DrawCircleSector(destinationCirclePosition, scrollFieldHeight / 2, 0, 180, 16, BLUE);
        DrawCircleSector(destinationCirclePosition, scrollFieldHeight / 3, 0, 180, 16, BLACK);
    }
    if(IsKeyDown(KEY_D)) {
        DrawCircleSector(destinationCirclePosition, scrollFieldHeight / 2, 180, 360, 16, BLUE);
        DrawCircleSector(destinationCirclePosition, scrollFieldHeight / 3, 180, 360, 16, BLACK);
    }
    if(IsKeyDown(KEY_J)) {
        DrawCircleSector(destinationCirclePosition, scrollFieldHeight / 3, 0, 180, 16, RED);
    }
    if(IsKeyDown(KEY_F)) {
        DrawCircleSector(destinationCirclePosition, scrollFieldHeight / 3, 180, 360, 16, RED);
    }

    for (int i = currentBeatmap->noteCount - 1; i >= 0; i--)
    {
        DrawNote(currentBeatmap->notes + i);

        currentBeatmap->notes[i].position.x = destinationCircleOffset + currentBeatmap->notes[i].timing - songTimeElapsed;
    }    

    if(judgementSwitch == Miss && songTimeElapsed - (float)lastNoteTiming < 300) {
        DrawTexture(taikoMiss, -30, -50 + scrollFieldOffset, WHITE); // TODO: Animate this (fade in fade out or scale)
    }
    if(judgementSwitch == Great && songTimeElapsed - (float)lastNoteTiming < 300) {
        DrawTexture(taikoGreat, -50, -40 + scrollFieldOffset, WHITE); // TODO: Animate this (fade in fade out or scale)
    }
    if(judgementSwitch == Good && songTimeElapsed - (float)lastNoteTiming < 300) {
        DrawTexture(taikoGood, -50, -40 + scrollFieldOffset, WHITE); // TODO: Animate this (fade in fade out or scale)
    }

    DrawFPS(2, 0);
    DrawTextEx(GuiGetFont(),TextFormat("Misses: %i", missCounter), (Vector2) { 2, (float)screenHeight - 115 }, 20, 2, WHITE);
    DrawTextEx(GuiGetFont(), TextFormat("Good: %i", goodCounter), (Vector2) { 2, (float)screenHeight - 95 }, 20, 2, WHITE);
    DrawTextEx(GuiGetFont(), TextFormat("Great: %i", greatCounter), (Vector2) { 2, (float)screenHeight - 75 }, 20, 2, WHITE);
    DrawTextEx(GuiGetFont(), TextFormat("%ix", comboCounter), (Vector2) { 2, (float)screenHeight - 65 }, 68, 2, WHITE);

    DrawTextEx(GuiGetFont(), TextFormat("%.2f%%", accuracy),
               (Vector2) { (float)screenWidth - MeasureTextEx(GuiGetFont(), TextFormat("%.2f%%", accuracy), 68, 2).x,
                           (float)screenHeight - 65 },
               68, 2, WHITE);

    EndDrawing();
}

void DrawPlayfield() {
    DrawTexturePro(currentBeatmap->background, (Rectangle) { 0, 0, (float)screenWidth, (float)screenHeight },
                   (Rectangle) { 0, 0, (float)screenWidth, (float)screenHeight }, (Vector2) { 0, 0 }, 0,
                   WHITE);
    DrawRectangleGradientH(0, scrollFieldOffset, screenWidth, (int)scrollFieldHeight, Fade(GRAY, 0.8f), Fade(BLACK, 0.8f));
    DrawCircleGradient(0, screenHeight, 250, BLACK, BLANK);
    DrawCircleGradient(screenWidth, screenHeight, 250, BLACK, BLANK);
    DrawCircleV(destinationCirclePosition, scrollFieldHeight / 2, BLACK); // The destination circle
}

void DrawNote(Note* taikoNote) {
    if(taikoNote->position.x <= (float)screenWidth + scrollFieldHeight / 2 + 15 && taikoNote->position.x >= -scrollFieldHeight / 2 - 15) {
        if (taikoNote->bigNote) {
            DrawCircleV(taikoNote->position, scrollFieldHeight / 2 + 15, taikoNote->noteColor);
            DrawCircleLines((int)taikoNote->position.x, (int)taikoNote->position.y, scrollFieldHeight / 2 + 15, BLACK);

        } else {
            DrawCircleV(taikoNote->position, scrollFieldHeight / 2, taikoNote->noteColor);
            DrawCircleLines((int)taikoNote->position.x, (int)taikoNote->position.y, scrollFieldHeight / 2, BLACK);
        }
    }
}

void UpdateGamePlaying() {

    if(currentNote == 0) {
        accuracy = 100.f;
    } else {
        accuracy = 100.f * ((float)greatCounter + (float)goodCounter / 2.0f) / (float)(greatCounter + goodCounter + missCounter);
    }

    float noteTiming = currentBeatmap->notes[currentNote].timing;
    float deltaPress = fabsf(noteTiming - songTimeElapsed);

    int hit = (deltaPress < hitWindowMiss) && currentBeatmap->notes[currentNote].isPressed == 0;

    int isBluePressed = (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_K));
    int isRedPressed = (IsKeyPressed(KEY_F) || IsKeyPressed(KEY_J));


    if(songTimeElapsed - noteTiming > hitWindowGood) {
        // If no note pressed in time
        NoteMiss();
    }
    // Blue note logic
    else if(currentBeatmap->notes[currentNote].isBlue) {
        if(isBluePressed && hit){
            // If note is hit
            if(deltaPress < hitWindowGreat) {
                NoteHit(Great);
            } else if(deltaPress < hitWindowGood){
                NoteHit(Good);
            } else {
                NoteMiss();
            }
        } else if(isRedPressed && hit){
            // If wrong note is pressed
            NoteMiss();
        }
    // Red note logic
    } else {
        if(isRedPressed && hit) {
            // If note is hit
            if(deltaPress < hitWindowGreat) {
                NoteHit(Great);
            } else if(deltaPress < hitWindowGood){
                NoteHit(Good);
            } else {
                NoteMiss();
            }
        } else if(isBluePressed && hit) {
            // If wrong note is pressed
            NoteMiss();
        }
    }
    if(currentBeatmap->notes[0].timing - songTimeElapsed > 1000) {
        if(IsKeyPressed(KEY_SPACE)) {
            SeekMusicStream(currentBeatmap->audio, (currentBeatmap->notes[0].timing - 1500) / 1000);
        }
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
        StopMusicStream(currentBeatmap->audio);
        gameStateSwitch = Menu;
        lastGameState = Menu;

        ResetGameplayVariables();
    }
}

void RetryButton() {
    if(IsMusicStreamPlaying(currentBeatmap->audio)) {
        StopMusicStream(currentBeatmap->audio);
        PlayMusicStream(currentBeatmap->audio);

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

void NoteHit(int judgement) {
    currentBeatmap->notes[currentNote].isPressed = 1;
    currentBeatmap->notes[currentNote].noteColor = currentBeatmap->notes[currentNote].isBlue?Fade(BLUE, 0.4f):Fade(RED, 0.4f);

    if(judgement == Great) {
        greatCounter++;
    } else if(judgement == Good) {
        goodCounter++;
    }
    comboCounter++;
    judgementSwitch = judgement;
    lastNoteTiming = currentBeatmap->notes[currentNote].timing;
    currentNote++;
}

void NoteMiss() {
    PlaySound(comboBreak);

    currentBeatmap->notes[currentNote].isPressed = 1;
    currentBeatmap->notes[currentNote].noteColor = BLANK;

    lastNoteTiming = currentBeatmap->notes[currentNote].timing;
    judgementSwitch = Miss;
    comboCounter = 0;
    currentNote++;
    missCounter++;
}
