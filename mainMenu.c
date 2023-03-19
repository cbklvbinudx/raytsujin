#include <string.h>
#include <stdlib.h>
#include "raylib.h"
#include "mainMenu.h"
#include "config.h"
#include "raygui.h"
#include "beatmap.h"


void DrawMainMenu() {
    BeginDrawing();

    ClearBackground(WHITE);

    if(currentBeatmap)
        DrawTexturePro(currentBeatmap->background, (Rectangle) { 0, 0, screenWidth, screenHeight },
                       (Rectangle) { 0, 0, screenWidth, screenHeight }, (Vector2) { 0, 0 }, 0,
                       WHITE);

    DrawTextEx(GetFontDefault(), "RAYTSUJIN",(Vector2) { 10,screenHeight - 86 }, 100, 8,GRAY);

    DrawRectangleGradientV(0, 0, screenWidth, screenHeight, BLACK, BLANK);

    GuiButton((Rectangle){ screenWidth / 2 - screenWidth / 3 / 2, screenHeight / 2 - screenHeight / 7 / 2 , screenWidth / 3, screenHeight / 7 },
                                "Drop a file with the .osu extension on the window to play!");

    if(currentBeatmap) {

        DrawText(TextFormat("Currently loaded map: %s - %s [%s]", currentBeatmap->artist, currentBeatmap->title, currentBeatmap->difficultyName), 2, 2, 20, GRAY);

        gameStateSwitch = GuiButton((Rectangle){ screenWidth / 2 - screenWidth / 3 / 2, screenHeight / 2 - screenHeight / 7 / 2, screenWidth / 3, screenHeight / 7 },
                                    "File loaded! Press this button or ENTER to play.");
    }

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
        }

        UnloadDroppedFiles(droppedFiles);
    }
}
