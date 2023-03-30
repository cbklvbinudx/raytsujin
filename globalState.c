#include "globalState.h"
#include "raylib.h"
#include "raygui.h"
#include "config.h"

void DisplayVolume(float volume, double lastChanged) {
    if(GetTime() - lastChanged < 0.5f && lastChanged != 0) {
        DrawTextEx(GuiGetFont(), TextFormat("Volume: %.2f", volume),
                   (Vector2) { (float)screenWidth - MeasureTextEx(GuiGetFont(), TextFormat("Volume: %.2f", volume), 36, 2).x - 10,
                               (float)5 },
                   36, 2, WHITE);
    }
}