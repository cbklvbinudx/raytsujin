#include "raylib.h"
#include "finishScreen.h"
#include "raygui.h"
#include "config.h"

int backButtonSwitch = 0;

void DrawFinishScreen() {
    BeginDrawing();

    ClearBackground(WHITE);
    DrawText("Finished!", 100, 80, 48, GRAY);

    DrawText(TextFormat("Great: %i", greatCounter), 100, 125, 20, GRAY);
    DrawText(TextFormat("Good: %i", goodCounter), 100, 145, 20, GRAY);
    DrawText(TextFormat("Misses: %i", missCounter), 100, 165, 20, GRAY);

    backButtonSwitch = GuiButton((Rectangle){ 100, screenHeight - 200, screenWidth / 3, screenHeight / 7 },
                                           "Back");

    EndDrawing();
}

void UpdateFinishScreen() {
    if(backButtonSwitch) gameStateSwitch = Menu;
    if(IsKeyPressed(KEY_ESCAPE)) gameStateSwitch = Menu;
}