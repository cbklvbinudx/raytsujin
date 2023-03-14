#include "raylib.h"
#include "beatmapInfo.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "osuProcessing.h"
#include "config.h"

int noteCounter = 0;
Note Notes[512] = { 0 }; // TODO: Make the size dynamic

enum sectionName {
    General,
    Metadata,
    Difficulty,
    Events,
    TimingPoints,
    HitObjects,
};

int sectionSwitch = 0;

Beatmap beatmap;

void StartOsuFileProcessing(char* fileName) {
    FILE* filePointer;

    char line[512] = { 0 };

    filePointer = fopen(fileName, "r");

    if(!filePointer) {
        noteCounter = -1;
    }

    // Initialized at the right end of the screen
    Vector2 notePosition = { 800.0f, 50.0f + scrollFieldOffset };

    while(fgets(line, 512, filePointer)) {

        if(strstr(line, "[General]")) {
            sectionSwitch = General;
            continue;
        }
        else if(strstr(line, "[Metadata]")) {
            sectionSwitch = Metadata;
            continue;
        }
        else if(strstr(line, "[Difficulty]")) {
            sectionSwitch = Difficulty;
            continue;
        }
        else if(strstr(line, "[Events]")) {
            sectionSwitch = Events;
            continue;
        }
        else if(strstr(line, "[TimingPoints]")) {
            sectionSwitch = TimingPoints;
            continue;
        }
        else if(strstr(line, "[HitObjects]")) {
            sectionSwitch = HitObjects;
            continue;
        }

        if(strstr(line, "AudioFilename: ") && sectionSwitch == General) {

            char* spaceSeperator = strtok(line, " ");
            spaceSeperator = strtok(NULL, " ");
            spaceSeperator[strlen(spaceSeperator) - 1] = '\0';

            char* audioName = malloc(strlen(spaceSeperator) + 1);
            strcpy(audioName, spaceSeperator);
            beatmap.audioFileName = audioName;
        }
        else if(sectionSwitch == Metadata) {
            if (strstr(line, "Title:")) {
                beatmap.title = GetBeatmapInfoString(line);
            } else if (strstr(line, "Artist:")) {
                beatmap.artist = GetBeatmapInfoString(line);
            } else if (strstr(line, "Version:")) {
                beatmap.difficultyName = GetBeatmapInfoString(line);
            }
        }
        else if(sectionSwitch == Events) {
            if(strstr(line, "0,0,")) {

                char* spaceSeperator = strtok(line, "\"");
                spaceSeperator = strtok(NULL, "\"");

                char* backgroundName = malloc(strlen(spaceSeperator) + 1);
                strcpy(backgroundName, spaceSeperator);

                beatmap.backgroundFileName = backgroundName;
            }
        }
        else if(sectionSwitch == Difficulty) {
            if(strstr(line, "HPDrainRate:")) {
                beatmap.hpDrain = GetBeatmapInfoInt(line);
            }
            else if(strstr(line, "OverallDifficulty:")) {
                beatmap.od = GetBeatmapInfoInt(line);
            }
        }
        // Filling info about hitobjects
        if(sectionSwitch == HitObjects) {

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
                    Notes[noteCounter].bigNote = 0;
                }
                else if(*commaSection == '4') {
                    Notes[noteCounter].isBlue = 0;
                    Notes[noteCounter].bigNote = 1;
                } else if(*commaSection == '8' || strcmp(commaSection, "12") == 0) {
                    Notes[noteCounter].isBlue = 1;
                    Notes[noteCounter].bigNote = 1;
                }
                else {
                    Notes[noteCounter].isBlue = 1;
                    Notes[noteCounter].bigNote = 0;
                }

                Notes[noteCounter].noteColor = Notes[noteCounter].isBlue?BLUE:RED;
                noteCounter++;
        }
    }
    fclose(filePointer);
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
