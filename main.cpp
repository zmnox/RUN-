#include <raylib.h>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdlib.h>

#include "hFiles/button.h"
#include "hFiles/score.h"
#include "hFiles/bird.h"
#include "hFiles/player.h"
#include "hFiles/platform.h"
#include "hFiles/collision.h"
#include "hFiles/initialize.h"
#include "hFiles/gamescreens.h"



//-----------------------------------------------------------------------------------------------------------------
// Program main entry point
//-----------------------------------------------------------------------------------------------------------------

int main(void){
//Initialization---------------------------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    player ply;
    platform p[MAX_P];
    bird birds[MAX_BIRDS];
    float speed = 0.0f;
    float gaps = 0.0f;
    int score = 0;
    float countdown = 3.0f;
    int hiScore = LoadHighScore();
    float scoreTimer = 0.0f;
    bool gameOver = false;

    initGame(&ply,p,birds,screenWidth,&speed,&gaps,&score);
    ///------------------------------------------------------------------------------------------------------------------------

    InitWindow(screenWidth,screenHeight,"RUN!!!");
    Texture2D background = LoadTexture("visuals/background.png");
    Texture2D dino = LoadTexture("visuals/Running.png");
    Texture2D birdTexture = LoadTexture("visuals/bird.png");
    Texture2D buildTexture = LoadTexture("visuals/build.png");
    
    Texture2D title = LoadTexture("visuals/Title.png");
    Texture2D scene1 = LoadTexture("visuals/Scene1.png");
    Texture2D scene2 = LoadTexture("visuals/Scene2.png");
    Texture2D timer1 = LoadTexture("visuals/Timer1.png");
    Texture2D timer2 = LoadTexture("visuals/Timer2.png");
    Texture2D timer3 = LoadTexture("visuals/Timer3.png");
    Texture2D L = LoadTexture("visuals/L.png");

    Rectangle recFrame = {0,0,(float)dino.width/8,(float)dino.height};
    int currentFrame = 0;
    float frameTime = 0.0f;
    float frameSpeed = 0.1f;
    gameScreen currentScreen = Title;

    Button startButton("Buttons/normalPlay.png", "Buttons/hoverPlay.png", "Buttons/clickedPlay.png", {100,250}, 0.15);
    Button quit1Button("Buttons/normalQuit.png", "Buttons/hoverQuit.png", "Buttons/clickedQuit.png", {100,320}, 0.15);
    Button resumeButton("Buttons/normalResume.png", "Buttons/hoverResume.png", "Buttons/clickedResume.png", {340,160}, 0.15);
    Button homeButton("Buttons/normalHome.png", "Buttons/hoverHome.png", "Buttons/clickedHome.png", {340,220}, 0.15);
    Button quit2Button("Buttons/normalQuit.png", "Buttons/hoverQuit.png", "Buttons/clickedQuit.png", {340,280}, 0.15);
    SetTargetFPS(60);

    InitAudioDevice();
    Sound countDownSound = LoadSound("audio/timer.mp3");
    Sound buttonSound = LoadSound("audio/buttonClick.mp3");
    Sound hundredSound = LoadSound("audio/hundred.mp3");
    Sound gameOverSound = LoadSound("audio/gameOver.mp3");
    Sound scene1Sound = LoadSound("audio/scene1.mp3");
    Sound scene2Sound = LoadSound("audio/scene2.mp3");
    Sound runSound = LoadSound("audio/RUN.mp3");
    Music bgMusic = LoadMusicStream("audio/bg.mp3");
    SetSoundVolume(countDownSound, 0.3f);
    int lastHundred = 0;
    bool gameOverSoundPlayed = false;


//MAIN GAME WINDOW--------------------------------------------------------------------------------------------------------------------------
    while(!WindowShouldClose()){
        UpdateMusicStream(bgMusic);
        frameTime += GetFrameTime();
        if(frameTime >= frameSpeed){
            frameTime = 0.0f;
            currentFrame++;
            if(currentFrame >= 8){
                currentFrame = 0;
            }
            recFrame.x = currentFrame * recFrame.width;
        }
    
        if(currentScreen == Title){
            startButton.Update();
            quit1Button.Update();
            if(startButton.isClicked()){
                PlaySound(buttonSound);
                PlaySound(scene1Sound);
                initGame(&ply,p,birds,screenWidth,&speed,&gaps,&score);
                scoreTimer = 0.0f;
                score = 0;
                lastHundred = 0;
                gameOver = false;
                countdown = 3.0f;
                currentScreen = Scene1;
            }
            else if(quit1Button.isClicked()){
                PlaySound(buttonSound);
                CloseWindow();
            }
        }
        else if(currentScreen == Scene1){
            if(IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                PlaySound(scene2Sound);
                currentScreen = Scene2;
            }
        }
        else if(currentScreen == Scene2){
            StopSound(scene1Sound);
            if(IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                StopSound(scene2Sound);
                currentScreen = Timer;
                PlaySound(countDownSound);
            }
        }
        else if(currentScreen == Timer){
            countdown -= GetFrameTime();
            if (countdown <0.0f){
                PlaySound(runSound);
                PlayMusicStream(bgMusic);
                currentScreen = Gameplay;
            }
        }
        else if(currentScreen == Gameplay){
            if(!gameOver){
                frameTime += GetFrameTime();
                if(frameTime >= frameSpeed){
                    frameTime = 0.0f;
                    currentFrame++;
                    if(currentFrame >= 8){
                        currentFrame = 0;
                    }
                    recFrame.x = currentFrame * recFrame.width;
                }
                scoreTimer += GetFrameTime();
                score = (int)(scoreTimer*10);

                if(score == lastHundred + 100){
                    lastHundred += 100;
                    PlaySound(hundredSound);
                }

                platformUpdate(&speed,p,birds,gaps,&score);
                updateBirds(birds, speed);
                playerUpdate(&ply);
                checkCollision(&ply,p);
                checkGameOver(&ply, birds, screenHeight, &gameOver);

                if(IsKeyPressed(KEY_P)){
                    PauseMusicStream(bgMusic);
                    currentScreen = Pause;
                }
            }
            else{
                if(score > hiScore){
                    hiScore = score;
                    SaveHighScore(hiScore);
                }

                if(gameOver && !gameOverSoundPlayed){
                    StopMusicStream(bgMusic);
                    PlaySound(gameOverSound);
                    gameOverSoundPlayed = true;
                }

                if(IsKeyPressed(KEY_ENTER)){
                    StopSound(gameOverSound);
                    initGame(&ply,p,birds,screenWidth,&speed,&gaps,&score);
                    scoreTimer = 0.0f;
                    score = 0;
                    lastHundred = 0;
                    countdown = 3.0f;
                    gameOver = false;
                    gameOverSoundPlayed = false;
                    SeekMusicStream(bgMusic, 0.0f);
                    PlayMusicStream(bgMusic);
                    currentScreen = Gameplay;
                }
            }
        }
        else if(currentScreen == Pause){
            
            if(IsKeyPressed(KEY_P)){
                StopSound(runSound);
                ResumeMusicStream(bgMusic);
                currentScreen = Gameplay;
            }

            resumeButton.Update();
            homeButton.Update();
            quit2Button.Update();

            if(resumeButton.isClicked()){
                PlaySound(buttonSound);
                ResumeMusicStream(bgMusic);
                StopSound(runSound);
                currentScreen = Gameplay;
            }
            else if(homeButton.isClicked()){
                PlaySound(buttonSound);
                StopSound(runSound);
                StopMusicStream(bgMusic);
                currentScreen = Title;
            }
            else if(quit2Button.isClicked()){
                PlaySound(buttonSound);
                StopSound(runSound);
                StopMusicStream(bgMusic);
                CloseWindow();
            }
        }

//VISUALIZE OF THE GAME--------------------------------------------------------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(RAYWHITE);

        switch(currentScreen){
            case Title:{
                DrawTexture(title,0,0,WHITE); 
                startButton.Draw();
                quit1Button.Draw();
            } break;

            case Scene1:{
                DrawTexture(scene1,0,0,WHITE);   
            } break;

            case Scene2:{
                DrawTexture(scene2,0,0,WHITE);
            } break;
            
            case Timer:{
                if(countdown > 2.0f){
                    DrawTexture(timer3,0,0,WHITE);
                }
                else if(countdown > 1.0f){
                    DrawTexture(timer2,0,0,WHITE);
                }
                else if(countdown > 0.0f){
                    DrawTexture(timer1,0,0,WHITE);
                }
            } break;

            case Gameplay:
            case Pause:{
                DrawTexture(background,0,0,WHITE);
                if(!gameOver){
                        drawPlatforms(p, buildTexture);
                        drawBirds(birds, birdTexture);
                        DrawText(TextFormat("SCORE: %i",score),620, 10, 20, BLACK);
                        DrawText(TextFormat("HIGH SCORE: %i",hiScore),400, 10, 20, BLACK);
                        Rectangle recDest = {ply.position.x, ply.position.y, ply.size.x, ply.size.y};
                        DrawTexturePro(dino,recFrame, recDest,{0,0},0.0f,WHITE);
                }
                else{
                        DrawRectangle(0,0,screenWidth,screenHeight,BLACK);
                        Rectangle source = {0, 0, (float)L.width, (float)L.height};
                        Rectangle dest = {screenWidth - 110, 10, 100, 100};
                        DrawTexturePro(L, source, dest, {0, 0}, 0.0f, WHITE);
                        DrawText("PRESS [ENTER] TO PLAY AGAIN", GetScreenWidth()/2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN",20)/2, GetScreenHeight()/2 - 20, 20, WHITE);
                }

                if(currentScreen == Pause){
                    DrawText("PAUSED", screenWidth/2 - MeasureText("PAUSED", 30)/2, 115, 30, WHITE);
                    resumeButton.Draw();
                    homeButton.Draw();
                    quit2Button.Draw();
                }
            }break;
        }
        
        EndDrawing();
    } 

UnloadTexture(dino);
UnloadTexture(birdTexture);
UnloadTexture(buildTexture);
UnloadTexture(background);
UnloadTexture(scene1);
UnloadTexture(scene2);
UnloadTexture(timer1);
UnloadTexture(timer2);
UnloadTexture(timer3);
UnloadSound(countDownSound);
UnloadSound(buttonSound);
UnloadSound(hundredSound);
UnloadSound(gameOverSound);
UnloadSound(scene1Sound);
UnloadSound(scene2Sound);
UnloadSound(runSound);
UnloadMusicStream(bgMusic);
CloseAudioDevice();

CloseWindow();
return 0;
}
