#pragma once
#include <raylib.h>

typedef struct{
    bool onGround;
    bool canJump;
}state;

typedef struct{
    Vector2 position;
    Vector2 size;
    state movement;
    float gravity;
    float jumpBuffer;
    float coyoteTime;
    bool isDucking;
}player;

void playerUpdate(player *ply);