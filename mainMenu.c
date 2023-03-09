#define RAYGUI_IMPLEMENTATION
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

    ClearBackground(RAYWHITE);

    gameStateSwitch = GuiButton((Rectangle){ screenWidth / 3, screenWidth / 7 , screenWidth / 3, screenHeight / 7 },
                                "Drop a file with the .osu extension on the window to play!;");

    if(isFileProcessed) {
        DrawText(TextFormat("Currently loaded file: %s", GetFileNameWithoutExt(extractedFilePath)), 2, 0, 20, GRAY);
    }

    EndDrawing();
}

void UpdateMainMenu() {
    if(IsKeyPressed(KEY_ENTER)) {
        gameStateSwitch = Playing;
    }
    if(IsFileDropped()) {
        droppedFiles = LoadDroppedFiles();
        if(droppedFiles.count == 1 && IsFileExtension(droppedFiles.paths[0], ".osu")) {

            if(strcmp(previousExtractedFilePath, extractedFilePath) != 0) {
                isFileProcessed = 0;
            }

            extractedFilePath = droppedFiles.paths[0];

            if(!isFileProcessed || strcmp(previousExtractedFilePath, extractedFilePath) != 0) {
                StartOsuFileProcessing(extractedFilePath);

                char *mapAudioBuffer = malloc(
                        strlen(GetPrevDirectoryPath(extractedFilePath)) + strlen(beatmap.audioFileName) + 1); // Causes a memory leak :D
                strcpy(mapAudioBuffer, GetPrevDirectoryPath(extractedFilePath));
                strcat(mapAudioBuffer, "\\");
                strcat(mapAudioBuffer, beatmap.audioFileName);
                mapAudio = LoadMusicStream(mapAudioBuffer);

                char *mapBackgroundBuffer = malloc(
                        strlen(GetPrevDirectoryPath(extractedFilePath)) + strlen(beatmap.backgroundFileName) + 1); // Causes a memory leak :D
                strcpy(mapBackgroundBuffer, GetPrevDirectoryPath(extractedFilePath));
                strcat(mapBackgroundBuffer, "\\");
                strcat(mapBackgroundBuffer, beatmap.backgroundFileName);
                mapBackground = LoadTexture(mapBackgroundBuffer);

                previousExtractedFilePath = droppedFiles.paths[0];
                isFileProcessed = 1;
            }
        }
    }
}