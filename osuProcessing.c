#include "raylib.h"
#include "note.h"
#include <stdio.h>
#include <string.h>

Note Notes[512] = { 0 };
int noteCounter = 0;
int commaCounter = 0;
char* goofyasstext = " ";

// Initialized at the right end of the screen
Vector2 notePosition = { 800.0f, 50.0f };

void StartOsuFileProcessing(char* fileName) {
    FILE* filePointer;

    char line[512] = { 0 };

    filePointer = fopen(fileName, "r");

    if(!filePointer) {
        noteCounter = -1;
    }

    while(fgets(line, 512, filePointer)) {

        goofyasstext = line;
        // char* ptr = strtok(line, ",");

        // while(ptr != NULL) {
        //     ptr = strtok(NULL, ",");
        // }

        Notes[noteCounter].timing = line[1];
        Notes[noteCounter].position = notePosition;
        Notes[noteCounter].isPressed = 0;
        Notes[noteCounter].noteColor = Notes[noteCounter].isBlue?BLUE:RED;
        Notes[noteCounter].sliderVelocity = 5;
        if(line[4] == '0') {
            Notes[noteCounter].isBlue = 0;
        }
        else {
            Notes[noteCounter].isBlue = 1;
        }
        noteCounter++;
    }

    fclose(filePointer);
}