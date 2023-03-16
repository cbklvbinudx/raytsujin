#include "raylib.h"
#include "beatmap.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "config.h"

int noteCount = 0;
Note *notes = NULL;

enum SectionEnum {
    General,
    Metadata,
    Difficulty,
    Events,
    TimingPoints,
    HitObjects,
};

int currentSection = 0;

Beatmap* currentBeatmap;

Beatmap* LoadBeatmapFromFile(char* fileName) {
    Beatmap* beatmap = malloc(sizeof(Beatmap));
    FILE* filePointer;

    char line[512] = { 0 };

    filePointer = fopen(fileName, "r");

    if(!filePointer) {
        noteCount = -1;
    }

    if(notes == NULL) {
        notes = malloc(20000 * sizeof(Note));
    }

    // Initialized at the right end of the screen
    Vector2 notePosition = { 800.0f, 50.0f + scrollFieldOffset };

    while(fgets(line, 512, filePointer)) {

        if(strstr(line, "[General]")) {
            currentSection = General;
            continue;
        }
        else if(strstr(line, "[Metadata]")) {
            currentSection = Metadata;
            continue;
        }
        else if(strstr(line, "[Difficulty]")) {
            currentSection = Difficulty;
            continue;
        }
        else if(strstr(line, "[Events]")) {
            currentSection = Events;
            continue;
        }
        else if(strstr(line, "[TimingPoints]")) {
            currentSection = TimingPoints;
            continue;
        }
        else if(strstr(line, "[HitObjects]")) {
            currentSection = HitObjects;
            continue;
        }

        if(strstr(line, "AudioFilename: ") && currentSection == General) {

            char* spaceSeperator = strtok(line, " ");
            spaceSeperator = strtok(NULL, " ");
            spaceSeperator[strlen(spaceSeperator) - 1] = '\0';

#if defined(__linux__)
            spaceSeperator[strlen(spaceSeperator) - 1] = '\0'; // Linux detects two line endings here, so we have to strip another one
#endif

            char* audioName = malloc(strlen(spaceSeperator) + 1);
            strcpy(audioName, spaceSeperator);
            beatmap->audioFileName = audioName;
        }
        else if(currentSection == Metadata) {
            if (strstr(line, "Title:")) {
                beatmap->title = GetBeatmapInfoString(line);
            } else if (strstr(line, "Artist:")) {
                beatmap->artist = GetBeatmapInfoString(line);
            } else if (strstr(line, "Version:")) {
                beatmap->difficultyName = GetBeatmapInfoString(line);
            }
        }
        else if(currentSection == Events) {
            if(strstr(line, "0,0,")) {

                char* spaceSeperator = strtok(line, "\"");
                spaceSeperator = strtok(NULL, "\"");

                char* backgroundName = malloc(strlen(spaceSeperator) + 1);
                strcpy(backgroundName, spaceSeperator);

                beatmap->backgroundFileName = backgroundName;
            }
        }
        else if(currentSection == Difficulty) {
            if(strstr(line, "HPDrainRate:")) {
                beatmap->hpDrain = GetBeatmapInfoInt(line);
            }
            else if(strstr(line, "OverallDifficulty:")) {
                beatmap->od = GetBeatmapInfoInt(line);
            }
        }
        // Filling info about hitobjects
        if(currentSection == HitObjects) {

                char* commaSection = strtok(line, ",");
                commaSection = strtok(NULL, ",");
                commaSection = strtok(NULL, ",");
                // We move to the timing section with this (after the second comma)

                notes[noteCount].timing = strtol(commaSection, NULL, 10); // Converting the string to an integer
                notes[noteCount].position = notePosition; // Initialize the notes
                notes[noteCount].isPressed = 0;
                notes[noteCount].sliderVelocity = 1;

                commaSection = strtok(NULL, ",");
                commaSection = strtok(NULL, ",");
                // We move to the hitsound section with this (after the fifth comma)

                if(*commaSection == '0') {
                    notes[noteCount].isBlue = 0;
                    notes[noteCount].bigNote = 0;
                }
                else if(*commaSection == '4') {
                    notes[noteCount].isBlue = 0;
                    notes[noteCount].bigNote = 1;
                } else if(*commaSection == '6' || strcmp(commaSection, "12") == 0) {
                    notes[noteCount].isBlue = 1;
                    notes[noteCount].bigNote = 1;
                }
                else {
                    notes[noteCount].isBlue = 1;
                    notes[noteCount].bigNote = 0;
                }

                notes[noteCount].noteColor = notes[noteCount].isBlue?BLUE:RED;
                noteCount++;
        }
    }
    fclose(filePointer);
    currentSection = 0;
    return beatmap;
}

void FreeBeatmap(Beatmap* beatmap) {
    free(beatmap->audioFileName);
    free(beatmap->backgroundFileName);
    free(beatmap->difficultyName);
    free(beatmap->artist);
    free(beatmap->title);
}

int GetBeatmapInfoInt(char* line) {

    char* spaceSeperator = strtok(line, ":");
    spaceSeperator = strtok(NULL, ":");
    spaceSeperator[strlen(spaceSeperator) - 1] = '\0';

    return strtol(spaceSeperator, NULL, 10);
}

char* GetBeatmapInfoString(char* line) {

    char* spaceSeperator = strtok(line, ":");
    spaceSeperator = strtok(NULL, ":");
    spaceSeperator[strlen(spaceSeperator) - 1] = '\0';

    char* returnString = malloc(strlen(spaceSeperator) + 1);
    strcpy(returnString, spaceSeperator);

    return returnString;
}
