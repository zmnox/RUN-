#include "../hFiles/platform.h"

#include <algorithm>
#include <cmath>
#include "../hFiles/constants.h"

void platformUpdate(float *speed, platform p[], bird birds[], float gaps, int *score){
    (void)gaps;

    for(int i=0;i<MAX_P;i++){
            p[i].position.x -= *speed;
        } 

        static float speedTimer = 0;
        speedTimer += GetFrameTime();
        if(speedTimer >= 1.0f){
            *speed += SPEED_GAIN_PER_SECOND;
            speedTimer = 0;
        }

        float lastPlatform = 0;
        float lastPlatformY = 350.0f;
        for(int i=0;i<MAX_P;i++){
            float currPlatform = p[i].size.x + p[i].position.x;
            if(currPlatform > lastPlatform){
                lastPlatform = currPlatform;
                lastPlatformY = p[i].position.y;
            }
        }

        for(int i=0;i<MAX_P;i++){
            if(p[i].position.x < -p[i].size.x){
                generatePlatformAfter(&p[i], lastPlatform, lastPlatformY, *speed, *score, birds);
                lastPlatform = p[i].size.x + p[i].position.x;
                lastPlatformY = p[i].position.y;
            }
        }
}


jumpMetrics getJumpMetrics(float speed, float verticalDelta){
    jumpMetrics metrics;
    float upwardSpeed = -JUMP_VELOCITY;
    float discriminant = (upwardSpeed * upwardSpeed) + (2.0f * GRAVITY_ACCEL * verticalDelta);

    if(discriminant < 1.0f){
        discriminant = 1.0f;
    }

    metrics.maxJumpHeight = (upwardSpeed * upwardSpeed) / (2.0f * GRAVITY_ACCEL);
    metrics.airtimeFrames = (upwardSpeed + sqrtf(discriminant)) / GRAVITY_ACCEL;
    metrics.platformTravel = speed * metrics.airtimeFrames;

    // Use a conservative slice of the theoretical travel so slightly late jumps can still land.
    metrics.maxGap = std::max(126.0f, (metrics.platformTravel * 0.86f) - PLAYER_STAND_WIDTH - 12.0f);
    metrics.minGap = std::min(metrics.maxGap - 12.0f, std::max(90.0f, metrics.maxGap * 0.62f));

    // As speed rises, a longer platform gives the fixed dinosaur a real landing and recovery window.
    metrics.minPlatformLength = PLAYER_STAND_WIDTH + std::max(112.0f, speed * 25.0f);
    return metrics;
}

void generatePlatformAfter(platform *platformToPlace, float previousRight, float previousY, float speed, int score, bird birds[]){
    float difficulty = std::min(1.0f, score / 900.0f);
    float nextY = previousY + (float)GetRandomValue(-18, 24);
    nextY = std::max(PLATFORM_TOP_MIN, std::min(PLATFORM_TOP_MAX, nextY));

    jumpMetrics metrics = getJumpMetrics(speed, nextY - previousY);
    float gapMin = metrics.minGap;
    float gapMax = metrics.maxGap * (0.94f + difficulty * 0.05f);

    if(gapMax < gapMin + 20.0f){
        gapMax = gapMin + 20.0f;
    }

    float gap = (float)GetRandomValue((int)gapMin, (int)gapMax);
    float platformMin = metrics.minPlatformLength + (speed * 2.0f);
    float platformMax = platformMin + 132.0f + (difficulty * 125.0f);
    float width = (float)GetRandomValue((int)platformMin, (int)platformMax);

    platformToPlace->position.x = previousRight + gap;
    platformToPlace->position.y = nextY;
    platformToPlace->size.x = width;
    platformToPlace->size.y = 300;

    trySpawnBirdOnPlatform(platformToPlace, gap, metrics.maxGap, speed, score, birds);
}



void trySpawnBirdOnPlatform(const platform *plat, float gapBefore, float maxSafeGap, float speed, int score, bird birds[]){
    float leadSpace = std::max(112.0f, speed * 14.0f);
    float recoverySpace = std::max(50.0f, speed * 7.0f);
    float birdWidth = 140.0f;
    float birdHeightPx = birdWidth * (193.0f / 288.0f);
    float platformLift = 12.0f;
    float platformSurfaceY = plat->position.y - platformLift;
    float safeBirdLength = leadSpace + recoverySpace + birdWidth;
    float longPlatformLength = safeBirdLength + 24.0f;
    float extraLongPlatformLength = safeBirdLength + 100.0f;
    bool longPlatform = plat->size.x >= longPlatformLength;
    bool extraLongPlatform = plat->size.x >= extraLongPlatformLength;
    bool obstacleBeat = ((score / 80) % 3) != 1;
    int activeBirds = 0;

    for(int i=0; i<MAX_BIRDS; i++){
        if(birds[i].active){
            activeBirds++;
        }
    }

    if(activeBirds >= 2){
        return;
    }

    if(gapBefore > maxSafeGap * 0.88f){
        return;
    }

    if(!longPlatform){
        return;
    }

    if(!extraLongPlatform && !obstacleBeat){
        return;
    }

    for(int i=0; i<MAX_BIRDS; i++){
        if(birds[i].active){
            continue;
        }

        birds[i].active = true;
        birds[i].height = BirdHigh;
        birds[i].size = {birdWidth, birdHeightPx};

        float spawnMin = plat->position.x + leadSpace;
        float spawnMax = plat->position.x + plat->size.x - recoverySpace - birdWidth;
        birds[i].position.x = spawnMin + ((spawnMax - spawnMin) * 0.55f);

        birds[i].position.y = platformSurfaceY - birdHeightPx - PLAYER_DUCK_HEIGHT - 2.0f;
        return;
    }
}



void drawPlatforms(platform p[], Texture2D buildTexture){
    Rectangle source = {0.0f, 0.0f, (float)buildTexture.width, (float)buildTexture.height};
    float platformLift = 12.0f;

    for(int i=0; i<MAX_P ; i++){
        Rectangle dest = {p[i].position.x, p[i].position.y - platformLift, p[i].size.x, p[i].size.y + platformLift};
        DrawTexturePro(buildTexture, source, dest, {0.0f, 0.0f}, 0.0f, WHITE);
    }
}

