#include "raylib.h"
#include "finishScreen.h"
#include "raygui.h"
#include "config.h"
#include "beatmap.h"

int backButtonSwitch = 0;

void DrawFinishScreen() {
    BeginDrawing();

    DrawTexturePro(currentBeatmap->background, (Rectangle) { 0, 0, (float)screenWidth, (float)screenHeight },
                   (Rectangle) { 0, 0, (float)screenWidth, (float)screenHeight }, (Vector2) { 0, 0 }, 0,
                   WHITE);
    DrawRectangleGradientV(0, 0, screenWidth, screenHeight, BLACK, BLANK);

    DrawTextEx(GuiGetFont(), "Finished!", (Vector2) { 100, 55 }, 48, 2, WHITE);

    DrawTextEx(GuiGetFont(), TextFormat("Great: %i", greatCounter), (Vector2) { 100, 100 }, 20, 2, WHITE);
    DrawTextEx(GuiGetFont(), TextFormat("Good: %i", goodCounter), (Vector2) { 100, 120 }, 20, 2, WHITE);
    DrawTextEx(GuiGetFont(), TextFormat("Misses: %i", missCounter), (Vector2) { 100, 140 }, 20, 2, WHITE);

    DrawTextEx(GuiGetFont(), TextFormat("%.2f%%", accuracy), (Vector2) { 100, 160 }, 68, 2, WHITE);

    backButtonSwitch = GuiButton((Rectangle){ 100, (float)screenHeight - 200, (float)screenWidth / 3, (float)screenHeight / 7 },
                                           "Back");

    EndDrawing();
}

void UpdateFinishScreen() {
    if(backButtonSwitch || IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_ENTER)) {
        ResetGameplayVariables();
        gameStateSwitch = Menu;
    }
}
