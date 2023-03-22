#pragma once
#include "beatmap.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef float  f32;
typedef double f64;

typedef struct IntDoublePair {
	u32 Int;
	f64 Double;
} IntDoublePair;

typedef struct TimingPoint {
	f64 bpm;
	f64 offset;
	u8 notInherited;
} TimingPoint;

typedef struct DateTime {
	u64 ticks;
} DateTime;

typedef struct OsuDBBeatmap {
	u32 size;
	const char* artistName;
	const char* artistNameUnicode;
	const char* songTitle;
	const char* songTitleUnicode;
	const char* creator;
	const char* difficulty;
	const char* audioFileName;
	const char* hash;
	const char* osuFileName;
	u8 rankedStatus;
	u16 hitcircleCount;
	u16 sliderCount;
	u16 spinnerCount;
	u64 lastModificationTime;
	f32 approachRate;
	f32 circleSize;
	f32 hpDrain;
	f32 overallDifficulty;
	f64 sliderVelocity;
	IntDoublePair* standardStarRatinngs;
	IntDoublePair* taikoStarRatinngs;
	IntDoublePair* ctbStarRatinngs;
	IntDoublePair* maniaStarRatinngs;
	u32 drainTime;
	u32 totalTime;
	u32 audioPreviewTime;
	u32 timingPointCount;
	TimingPoint* timingPoints;
	u32 difficultyID;
	u32 beatmapID;
	u32 threadID;
	u8 stdGrade;
	u8 taikoGrade;
	u8 ctbGrade;
	u8 maniaGrade;
	u16 localOffset;
	f32 stackLeniency;
	u8 mode;
	const char* songSource;
	const char* songTags;
	u16 onlineOffset;
	const char* titleFont;
	u8 played;
	u64 lastTimePlayed;
	u8 isOsz2;
	const char* directory;
	u64 lastTimeUpdated;
	u8 ignoreBeatmapHitsounds;
	u8 ignoreBeatmapSkin;
	u8 disableStoryboard;
	u8 disableVideo;
	u8 visualOverride;
	u32 unknownInt;
	u8 maniaScrollSpeed;
} OsuDBBeatmap;

typedef struct OsuDB {
	u32 version;
	u32 folderCount;
	u8 accountLocked;
	DateTime accountUnlockTime;
	const char* playerName;
	u32 beatmapCount;
	Beatmap* beatmaps;
	u32 userPermissions;
} OsuDB;

u8  readByte(FILE* file);
u16 readShort(FILE* file);
u32 readInt(FILE* file);
u64 readLong(FILE* file);

f32 readSingle(FILE* file);
f64 readDouble(FILE* file);

bool readBool(FILE* file);
u64 readULEB128(FILE* file);

const char* readString(FILE* file);

IntDoublePair readIntDoublePair(FILE* file);
TimingPoint   readTimingPoint(FILE* file);
DateTime      readDateTime(FILE* file);

