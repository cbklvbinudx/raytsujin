#define RAYGUI_IMPLEMENTATION
#include "raylib.h"
#include "mainMenu.h"
#include "config.h"
#include "raygui.h"

void DrawMainMenu() {
    BeginDrawing();

    ClearBackground(RAYWHITE);

    gameStateSwitch = GuiButton((Rectangle){ screenWidth / 3, screenWidth / 7 , screenWidth / 3, screenHeight / 7 },
                                TextFormat("Press to play %s", pathToDifficulty));
    EndDrawing();
}

void UpdateMainMenu() {
    if(IsKeyPressed(KEY_ENTER)) {
        gameStateSwitch = Playing;
    }
}