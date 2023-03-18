#include "raylib.h"
#include "finishScreen.h"
#include "raygui.h"
#include "config.h"
#include "beatmap.h"

int backButtonSwitch = 0;

void DrawFinishScreen() {
    BeginDrawing();

    DrawTexturePro(mapBackground, (Rectangle) { 0, 0, screenWidth, screenHeight },
                   (Rectangle) { 0, 0, screenWidth, screenHeight }, (Vector2) { 0, 0 }, 0,
                   WHITE);
    DrawRectangleGradientV(0, 0, screenWidth, screenHeight, BLACK, BLANK);

    DrawText("Finished!", 100, 55, 48, WHITE);

    DrawText(TextFormat("Great: %i", greatCounter), 100, 100, 20, WHITE);
    DrawText(TextFormat("Good: %i", goodCounter), 100, 120, 20, WHITE);
    DrawText(TextFormat("Misses: %i", missCounter), 100, 140, 20, WHITE);

    backButtonSwitch = GuiButton((Rectangle){ 100, screenHeight - 200, screenWidth / 3, screenHeight / 7 },
                                           "Back");

    EndDrawing();
}

void UpdateFinishScreen() {
    if(backButtonSwitch || IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_ENTER)) {
        ResetGameplayVariables();
        gameStateSwitch = Menu;
    }
}
