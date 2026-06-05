#pragma once

#include <raylib.h>

#define MAX_BIRDS 2

typedef struct{
    Vector2 position;
    Vector2 size;
    bool active;
}bird;

void clearBirds(bird birds[]);
void updateBirds(bird birds[], float speed);
void drawBirds(bird birds[], Texture2D birdTexture);
