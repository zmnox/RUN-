#pragma once

#include <raylib.h>
#include "bird.h"

#define MAX_P 4

typedef struct{ 
    Vector2 position;
    Vector2 size;
    float velocity;
}platform;

typedef struct{
    float maxJumpHeight;
    float airtimeFrames;
    float platformTravel;
    float minGap;
    float maxGap;
    float minPlatformLength;
}jumpMetrics;

void platformUpdate(float *speed, platform p[], bird birds[], float gaps, int *score);
jumpMetrics getJumpMetrics(float speed, float verticalDelta);
void generatePlatformAfter(platform *platformToPlace, float previousRight, float previousY, float speed, int score, bird birds[]);
void drawPlatforms(platform p[], Texture2D buildTexture);
void trySpawnBirdOnPlatform(const platform *plat, float gapBefore, float maxSafeGap, float speed, int score, bird birds[]);