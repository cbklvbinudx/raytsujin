#include "raylib.h"
#include "beatmap.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "config.h"


enum SectionEnum {
    General,
    Metadata,
    Difficulty,
    Events,
    TimingPoints,
    HitObjects,
};

int currentSection = 0;
long int hitObjectsPosition;

Beatmap* currentBeatmap = NULL;

Beatmap* LoadBeatmapFromFile(const char* fileName) {
    Beatmap* beatmap = malloc(sizeof(Beatmap));
    beatmap->noteCount = 0;

    FILE* filePointer;

    char line[512] = { 0 };

    filePointer = fopen(fileName, "r");


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
            hitObjectsPosition = ftell(filePointer);
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
        // Counting the amount of hitobjects in order to allocate enough memory later
        if (currentSection == HitObjects) {
            beatmap->noteCount++;
        }
    }
    // We finished counting the objects, so now we can allocate enough memory
    beatmap->notes = malloc(beatmap->noteCount * sizeof(Note));
    beatmap->noteCount = 0;
    // We skip to the [HitObjects] section and fill in the notes data
    fseek(filePointer, hitObjectsPosition, SEEK_SET);
    while (fgets(line, 512, filePointer)) {

        char *commaSection = strtok(line, ",");
        commaSection = strtok(NULL, ",");
        commaSection = strtok(NULL, ",");
        // We move to the timing section with this (after the second comma)

        beatmap->notes[beatmap->noteCount].timing = strtol(commaSection, NULL,
                                                           10); // Converting the string to an integer
        beatmap->notes[beatmap->noteCount].position = notePosition; // Initialize the notes
        beatmap->notes[beatmap->noteCount].isPressed = 0;
        beatmap->notes[beatmap->noteCount].sliderVelocity = 1;

        commaSection = strtok(NULL, ",");
        commaSection = strtok(NULL, ",");
        // We move to the hitsound section with this (after the fifth comma)

        if (*commaSection == '0') {
            beatmap->notes[beatmap->noteCount].isBlue = 0;
            beatmap->notes[beatmap->noteCount].bigNote = 0;
        } else if (*commaSection == '4') {
            beatmap->notes[beatmap->noteCount].isBlue = 0;
            beatmap->notes[beatmap->noteCount].bigNote = 1;
        } else if (*commaSection == '6' || strcmp(commaSection, "12") == 0) {
            beatmap->notes[beatmap->noteCount].isBlue = 1;
            beatmap->notes[beatmap->noteCount].bigNote = 1;
        } else {
            beatmap->notes[beatmap->noteCount].isBlue = 1;
            beatmap->notes[beatmap->noteCount].bigNote = 0;
        }

        beatmap->notes[beatmap->noteCount].noteColor = beatmap->notes[beatmap->noteCount].isBlue ? BLUE
                                                                                                 : RED;
        beatmap->noteCount++;
    }
    fclose(filePointer);
    currentSection = 0;
    return beatmap;
}

void FreeBeatmap(Beatmap* beatmap) {
    free(beatmap->notes);
    free(beatmap->audioFileName);
    free(beatmap->backgroundFileName);
    free(beatmap->difficultyName);
    free(beatmap->artist);
    free(beatmap->title);
    free(beatmap);
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
