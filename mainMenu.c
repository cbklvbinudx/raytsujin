#include "raylib.h"
#include "mainMenu.h"
#include "config.h"
#include "raygui.h"

void DrawMainMenu() {
    BeginDrawing();

    ClearBackground(RAYWHITE);

    DrawText(TextFormat("Press enter to play %s", pathToDifficulty), screenWidth / 2 - 50, screenHeight / 2, 12, BLACK);

    EndDrawing();
}

void UpdateMainMenu() {
    if(IsKeyPressed(KEY_ENTER)) {
        gameStateSwitch = Playing;
    }
}