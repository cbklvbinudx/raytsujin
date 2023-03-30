#include "raylib.h"
#include "mainMenu.h"
#include "config.h"
#include "raygui.h"
#include "beatmap.h"
#include "globalState.h"

void DrawMainMenu() {
    BeginDrawing();

    ClearBackground(WHITE);

    if(currentBeatmap)
        DrawTexturePro(currentBeatmap->background, (Rectangle) { 0, 0, (float)screenWidth, (float)screenHeight },
                       (Rectangle) { 0, 0, (float)screenWidth, (float)screenHeight }, (Vector2) { 0, 0 }, 0,
                       WHITE);

    DrawTextEx(GetFontDefault(), "RAYTSUJIN",(Vector2) { 10,(float)screenHeight - 86 }, 100, 8,GRAY);

    DrawRectangleGradientV(0, 0, screenWidth, screenHeight, BLACK, BLANK);

    Rectangle buttonRectangle = { (float)screenWidth / 2.0f - (float)screenWidth / 3.0f / 2.0f,
                                  (float)screenHeight / 2.0f - (float)screenHeight / 7.0f / 2.0f ,
                                  (float)screenWidth / 3.0f, (float)screenHeight / 7.0f};

    GuiButton(buttonRectangle,"Drop a file with the .osu extension on the window to play!");

    if(currentBeatmap) {
        DrawTextEx(GuiGetFont(), TextFormat("Currently loaded map: %s - %s [%s]", currentBeatmap->artist, currentBeatmap->title, currentBeatmap->difficultyName),
                   (Vector2) { 2, 2 }, 20, 2, GRAY);

        gameStateSwitch = GuiButton(buttonRectangle, "File loaded! Press this button or ENTER to play.");
    }

    DisplayVolume(currentVolume, changeTime);

    EndDrawing();
}

void UpdateMainMenu() {
    if(IsKeyPressed(KEY_ENTER)) {
        gameStateSwitch = Playing;
    }
    if(IsKeyPressed(KEY_ESCAPE)) {
        SetExitKey(KEY_ESCAPE);
    }
    if(IsKeyPressed(KEY_F) && !IsWindowFullscreen()) {
        screenWidth = GetMonitorWidth(GetCurrentMonitor());
        screenHeight = GetMonitorHeight(GetCurrentMonitor());
        SetWindowSize(screenWidth, screenHeight);
        ToggleFullscreen();
    } else if(IsKeyPressed(KEY_F) && IsWindowFullscreen()) {
        ToggleFullscreen();
        screenWidth = 1600;
        screenHeight = 900;
        SetWindowSize(screenWidth, screenHeight);
    }

    if(IsFileDropped()) {
        FilePathList droppedFiles = LoadDroppedFiles();

        if(droppedFiles.count == 1 && IsFileExtension(droppedFiles.paths[0], ".osu")) {
            if(currentBeatmap) FreeBeatmap(currentBeatmap);
            currentBeatmap = LoadBeatmapFromFile(droppedFiles.paths[0]);
            SetWindowTitle(TextFormat("%s - %s [%s]", currentBeatmap->artist, currentBeatmap->title, currentBeatmap->difficultyName));
        }

        UnloadDroppedFiles(droppedFiles);
    }
}
