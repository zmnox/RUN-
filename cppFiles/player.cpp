#include "../hFiles/player.h"
#include "../hFiles/constants.h"


void playerUpdate(player *ply){
    bool wantsDuck = (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) && ply->movement.onGround;

    if(wantsDuck && !ply->isDucking){
        ply->position.y += PLAYER_STAND_HEIGHT - PLAYER_DUCK_HEIGHT;
        ply->size.y = PLAYER_DUCK_HEIGHT;
        ply->isDucking = true;
    }
    else if(!wantsDuck && ply->isDucking){
        ply->position.y -= PLAYER_STAND_HEIGHT - PLAYER_DUCK_HEIGHT;
        ply->size.y = PLAYER_STAND_HEIGHT;
        ply->isDucking = false;
    }

    if ((IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) && !ply->isDucking){
        ply->jumpBuffer = 0.15f;
    }
    ply->jumpBuffer -= GetFrameTime();

    if (ply->movement.onGround){
        ply->coyoteTime = 0.1f; 
    } 
    else {
        ply->coyoteTime -= GetFrameTime();
    }

    if(ply->jumpBuffer > 0 && ply->coyoteTime > 0){
        if(ply->isDucking){
            ply->position.y -= PLAYER_STAND_HEIGHT - PLAYER_DUCK_HEIGHT;
            ply->size.y = PLAYER_STAND_HEIGHT;
            ply->isDucking = false;
        }
        ply->gravity = JUMP_VELOCITY;
        ply->movement.onGround = false;
        ply->jumpBuffer = 0;
    }

    ply->gravity += GRAVITY_ACCEL;
    
    if(ply->gravity > MAX_FALL_SPEED){
        ply->gravity = MAX_FALL_SPEED;
    }

    ply->position.y += ply->gravity;
    ply->movement.onGround = false;
}