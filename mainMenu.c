#include <string.h>
#include <stdlib.h>
#include "raylib.h"
#include "mainMenu.h"
#include "config.h"
#include "raygui.h"
#include "osuProcessing.h"

FilePathList droppedFiles;
char* previousExtractedFilePath = "";
char* extractedFilePath = "";
int isFileProcessed = 0;

void DrawMainMenu() {
    BeginDrawing();

    ClearBackground(WHITE);
    DrawTexturePro(mapBackground, (Rectangle) { 0, 0, screenWidth, screenHeight },
                   (Rectangle) { 0, 0, screenWidth, screenHeight }, (Vector2) { 0, 0 }, 0,
                   WHITE);

    DrawTextEx(GetFontDefault(), "RAYTSUJIN",(Vector2) { 10,screenHeight - 86 }, 100, 8,GRAY);

    DrawRectangleGradientV(0, 0, screenWidth, screenHeight, BLACK, BLANK);

    GuiButton((Rectangle){ screenWidth / 2 - screenWidth / 3 / 2, screenHeight / 2 - screenHeight / 7 / 2 , screenWidth / 3, screenHeight / 7 },
                                "Drop a file with the .osu extension on the window to play!");

    if(isFileProcessed) {

        DrawText(TextFormat("Currently loaded file: %s", GetFileNameWithoutExt(extractedFilePath)), 2, 2, 20, GRAY);

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

    if(IsFileDropped()) {

        droppedFiles = LoadDroppedFiles();

        if(droppedFiles.count == 1 && IsFileExtension(droppedFiles.paths[0], ".osu")) {

            char* extractedFilePathBuffer = malloc(strlen(droppedFiles.paths[0]) + 1);
            strcpy(extractedFilePathBuffer, droppedFiles.paths[0]);
            extractedFilePath = extractedFilePathBuffer;

            if(strcmp(previousExtractedFilePath, extractedFilePath) != 0 && isFileProcessed) {
                isFileProcessed = 0;
                ResetGameplayVariables();
                noteCounter = 0;
                FreeBeatmapMemory();
                UnloadTexture(mapBackground);
                UnloadMusicStream(mapAudio);
            }

            if(!isFileProcessed || strcmp(previousExtractedFilePath, extractedFilePath) != 0) {
                StartOsuFileProcessing(extractedFilePath);

                char *mapAudioBuffer = malloc(
                        strlen(GetPrevDirectoryPath(extractedFilePath)) + strlen(beatmap.audioFileName) + 2); // + 2 for the terminator and for the backslash
                strcpy(mapAudioBuffer, GetPrevDirectoryPath(extractedFilePath));
                strcat(mapAudioBuffer, "/");
                strcat(mapAudioBuffer, beatmap.audioFileName);
                mapAudio = LoadMusicStream(mapAudioBuffer);

                char *mapBackgroundBuffer = malloc(
                        strlen(GetPrevDirectoryPath(extractedFilePath)) + strlen(beatmap.backgroundFileName) + 2); // + 2 for the terminator and for the backslash
                strcpy(mapBackgroundBuffer, GetPrevDirectoryPath(extractedFilePath));
                strcat(mapBackgroundBuffer, "/");
                strcat(mapBackgroundBuffer, beatmap.backgroundFileName);

                Image mapBackgroundImage = LoadImage(mapBackgroundBuffer);
                ImageResize(&mapBackgroundImage, screenWidth, screenHeight);

                mapBackground = LoadTextureFromImage(mapBackgroundImage);
                UnloadImage(mapBackgroundImage);

                char* previousExtractedFilePathBuffer = malloc(strlen(droppedFiles.paths[0]) + 1);
                strcpy(previousExtractedFilePathBuffer, droppedFiles.paths[0]);
                previousExtractedFilePath = previousExtractedFilePathBuffer;

                isFileProcessed = 1;
            }
        }
    }
}