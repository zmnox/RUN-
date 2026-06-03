#pragma once

#include "player.h"
#include "platform.h"
#include "bird.h"


void checkCollision(player *ply, platform p[]);
void checkGameOver(player *ply, bird birds[], int screenHeight, bool *gameOver);
