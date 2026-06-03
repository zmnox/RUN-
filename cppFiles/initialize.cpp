#include "../hFiles/initialize.h"
#include "../hFiles/platform.h"
#include "../hFiles/bird.h"
#include "../hFiles/constants.h"
#include <algorithm>

void initGame(player *ply, platform p[], bird birds[], int screenWidth,float *speed, float *gaps, int *score){
    (void)screenWidth;
    (void)score;

    //player
    ply->position.x = PLAYER_X;
    ply->position.y = 300;
    ply->size.x = PLAYER_STAND_WIDTH;
    ply->size.y = PLAYER_STAND_HEIGHT;
    ply->gravity = 0;
    ply->movement.onGround = true;
    ply->movement.canJump = true;
    ply->jumpBuffer = 0.0f;
    ply->coyoteTime = 0.0f;
    ply->isDucking = false;


    //platform 
    *speed = BASE_SPEED;
    int origY = 350;
    float spawn = 0.0f;
        
    
    p[0].position.x = 0;
    p[0].position.y = origY;
    p[0].size.x = 245;
    p[0].size.y = 300;
    int platformWidth[4] = {245, 172, 190, 182};
   
    for(int i=1; i<MAX_P; i++){
        p[i].size.x = platformWidth[i];
        p[i].position.y = (float)GetRandomValue((int)PLATFORM_TOP_MIN, (int)PLATFORM_TOP_MAX);
        p[i].size.y = 300;
    }

    *gaps = 145.0f;
    
    spawn=0;
    for(int i=0; i<MAX_P; i++){
        p[i].position.x = spawn;
        jumpMetrics metrics = getJumpMetrics(*speed, 0.0f);
        spawn += p[i].size.x + std::min(*gaps, metrics.maxGap * 0.88f);
    }

    clearBirds(birds);
}
    
