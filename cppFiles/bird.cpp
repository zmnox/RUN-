#include "../hFiles/bird.h"

void clearBirds(bird birds[]){
    for(int i=0; i<MAX_BIRDS; i++){
        birds[i].active = false;
        birds[i].position = {-200.0f, -200.0f};
        birds[i].size = {140.0f, 94.0f};
    }
}

void updateBirds(bird birds[], float speed){
    for(int i=0; i<MAX_BIRDS; i++){
        if(!birds[i].active){
            continue;
        }

        birds[i].position.x -= speed;
        if(birds[i].position.x < -birds[i].size.x){
            birds[i].active = false;
        }
    }
}

void drawBirds(bird birds[], Texture2D birdTexture){
    Rectangle source = {0.0f, 0.0f, (float)birdTexture.width, (float)birdTexture.height};

    for(int i=0; i<MAX_BIRDS; i++){
        if(!birds[i].active){
            continue;
        }

        Rectangle dest = {birds[i].position.x, birds[i].position.y, birds[i].size.x, birds[i].size.y};
        DrawTexturePro(birdTexture, source, dest, {0.0f, 0.0f}, 0.0f, WHITE);
    }
}