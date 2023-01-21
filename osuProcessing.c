#include "raylib.h"
#include "note.h"
#include <stdio.h>

Note taikoNotes[] = { 0 };
int noteCounter = 0;

void StartOsuFileProcessing(char* fileName) {
    FILE* filePointer;

    char line[512] = { 0 };

    filePointer = fopen(fileName, "r");

    if(!filePointer) {
        noteCounter = -1;
    }

    while(fgets(line, 512, filePointer)) {
        noteCounter++;
    }

    fclose(filePointer);
}