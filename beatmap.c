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

float hitWindowGreat;
float hitWindowGood;
float hitWindowMiss;

Beatmap* currentBeatmap = NULL;

Beatmap* LoadBeatmapFromFile(const char* fileName) {
    Beatmap* beatmap = malloc(sizeof(Beatmap));
    memset(beatmap, 0, sizeof(Beatmap));

    int currentSection = 0;
    long int hitObjectsPosition;

    FILE* filePointer;

    char line[2048] = { 0 };

    filePointer = fopen(fileName, "r");


    // Initialized at the right end of the screen
    Vector2 notePosition = { 800.0f, 50.0f + (float)scrollFieldOffset };

    while(fgets(line, 2048, filePointer)) {

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

            char* spaceSeperator = strtok(line, ":");
            spaceSeperator = strtok(NULL, ":");
            spaceSeperator[strlen(spaceSeperator) - 1] = '\0';
            spaceSeperator += 1; // We delete the space in the first character

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
                beatmap->hpDrain = GetBeatmapInfoFloat(line);
            }
            else if(strstr(line, "OverallDifficulty:")) {
                beatmap->od = GetBeatmapInfoFloat(line);
                if(beatmap->od < 5) {
                    hitWindowGreat = 35.0f - (35.0f - 50.0f) * (5.0f - beatmap-> od) / 5.0f;
                    hitWindowGood = 80.0f - (80.0f - 120.0f) * (5.0f - beatmap->od) / 5.0f;
                    hitWindowMiss = 95.0f - (95.0f - 135.0f) * (5.0f - beatmap->od) / 5.0f;
                } else if(beatmap->od > 5) {
                    hitWindowGreat = 35.0f + (20.0f - 35.0f) * (beatmap->od - 5.0f) / 5.0f;
                    hitWindowGood = 80.0f + (50.0f - 80.0f) * (beatmap->od - 5.0f) / 5.0f;
                    hitWindowMiss = 95.0f + (70.0f - 95.0f) * (beatmap->od - 5.0f) / 5.0f;
                } else {
                    hitWindowGreat = 35;
                    hitWindowGood = 80;
                    hitWindowMiss = 95;
                }
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
    while (fgets(line, 2048, filePointer)) {

        char *commaSection = strtok(line, ",");
        commaSection = strtok(NULL, ",");
        commaSection = strtok(NULL, ",");
        // We move to the timing section with this (after the second comma)

        beatmap->notes[beatmap->noteCount].timing = strtof(commaSection, NULL); // Converting the string to a float
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

    const char* parentDirectory = GetPrevDirectoryPath(fileName);
    beatmap -> directory = malloc(strlen(parentDirectory) + 1);
    strcpy(beatmap->directory, parentDirectory);

    LoadBeatmapAudio(beatmap);
    LoadBeatmapBackground(beatmap);

    return beatmap;
}

void LoadBeatmapAudio(Beatmap* beatmap) {
    beatmap->audio = LoadMusicStream(TextFormat("%s/%s", beatmap->directory, beatmap->audioFileName));
}

void LoadBeatmapBackground(Beatmap* beatmap) {
    Image mapBackgroundImage = LoadImage(TextFormat("%s/%s", beatmap->directory, beatmap->backgroundFileName));
    ImageResize(&mapBackgroundImage, screenWidth, screenHeight);
    beatmap->background = LoadTextureFromImage(mapBackgroundImage);
    UnloadImage(mapBackgroundImage);
}

void FreeBeatmap(Beatmap* beatmap) {
    UnloadMusicStream(beatmap->audio);
    UnloadTexture(beatmap->background);
    free(beatmap->notes);
    free(beatmap->audioFileName);
    free(beatmap->backgroundFileName);
    free(beatmap->difficultyName);
    free(beatmap->artist);
    free(beatmap->title);
    free(beatmap->directory);
    free(beatmap);
}

float GetBeatmapInfoFloat(char* line) {

    char* spaceSeperator = strtok(line, ":");
    spaceSeperator = strtok(NULL, ":");
    spaceSeperator[strlen(spaceSeperator) - 1] = '\0';

    return strtof(spaceSeperator, NULL);
}

char* GetBeatmapInfoString(char* line) {

    char* spaceSeperator = strtok(line, ":");
    spaceSeperator = strtok(NULL, ":");
    spaceSeperator[strlen(spaceSeperator) - 1] = '\0';

    char* returnString = malloc(strlen(spaceSeperator) + 1);
    strcpy(returnString, spaceSeperator);

    return returnString;
}
