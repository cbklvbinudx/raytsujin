#include "raylib.h"
#include "note.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "osuProcessing.h"

int noteCounter = 0;
Note Notes[512] = { 0 }; // TODO: Make the size dynamic
int hitObjectsSwitch = 0;

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

        if(strstr(line, "[HitObjects]") || hitObjectsSwitch) {

            if(hitObjectsSwitch) {

                char* commaSection = strtok(line, ",");
                commaSection = strtok(NULL, ",");
                commaSection = strtok(NULL, ",");
                // We move to the timing section with this (after the second comma)

                Notes[noteCounter].timing = strtol(commaSection, NULL, 10); // Converting the string to an integer
                Notes[noteCounter].position = notePosition; // Initialize the notes
                Notes[noteCounter].isPressed = 0;
                Notes[noteCounter].sliderVelocity = 1;

                commaSection = strtok(NULL, ",");
                commaSection = strtok(NULL, ",");
                // We move to the hitsound section with this (after the fifth comma)

                if(*commaSection == '0') {
                    Notes[noteCounter].isBlue = 0;
                }
                else {
                    Notes[noteCounter].isBlue = 1;
                }

                Notes[noteCounter].noteColor = Notes[noteCounter].isBlue?BLUE:RED;
                noteCounter++;
            } 
            hitObjectsSwitch = 1; // We hit the HitObjects line
        }
    }
    fclose(filePointer);
}