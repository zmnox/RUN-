#include "../hFiles/collision.h"
#include <raylib.h>


void checkCollision(player *ply, platform p[]){
    for(int i=0; i<MAX_P; i++){
            float platformLift = 12.0f;
            Rectangle playerColl = {ply->position.x, ply->position.y, ply->size.x, ply->size.y};
            Rectangle platColl = {p[i].position.x, p[i].position.y - platformLift, p[i].size.x, p[i].size.y + platformLift};
            
            if(CheckCollisionRecs(playerColl,platColl)){
                if(ply->gravity>0){
                    float playerBott = ply->size.y + ply->position.y;
                        if (playerBott <= platColl.y + 20) {
                            ply->position.y = platColl.y - ply->size.y;
                            ply->gravity = 0;
                            ply->movement.onGround = true;
                    }
                }
            }
        }
}


void checkGameOver(player *ply, bird birds[], int screenHeight, bool *gameOver){ 
    if(ply->position.y > screenHeight){
        *gameOver = true;
        return;
    }

    Rectangle playerColl = {ply->position.x, ply->position.y, ply->size.x, ply->size.y};
    for(int i=0; i<MAX_BIRDS; i++){
        if(!birds[i].active){
            continue;
        }

        Rectangle birdColl = {birds[i].position.x, birds[i].position.y, birds[i].size.x, birds[i].size.y};
        if(CheckCollisionRecs(playerColl, birdColl)){
            *gameOver = true;
            return;
        }
    }
}

