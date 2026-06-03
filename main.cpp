#include <raylib.h>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdlib.h>

#define MAX_P 4
#define MAX_BIRDS 5

const float PLAYER_X = 90.0f;
const float PLAYER_STAND_WIDTH = 50.0f;
const float PLAYER_STAND_HEIGHT = 50.0f;
const float PLAYER_DUCK_HEIGHT = 30.0f;
const float PLATFORM_TOP_MIN = 310.0f;
const float PLATFORM_TOP_MAX = 380.0f;
const float BASE_SPEED = 3.6f;
const float SPEED_GAIN_PER_SECOND = 0.13f;
const float JUMP_VELOCITY = -14.5f;
const float GRAVITY_ACCEL = 0.62f;
const float MAX_FALL_SPEED = 13.5f;

typedef struct{ 
    Vector2 position;
    Vector2 size;
    float velocity;
}platform;

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

typedef enum{
    Title,
    Scene1,
    Scene2,
    Timer,
    Gameplay,
    Pause
}gameScreen;

typedef enum{
    BirdLow,
    BirdMedium,
    BirdHigh
}birdHeight;

typedef struct{
    Vector2 position;
    Vector2 size;
    bool active;
    birdHeight height;
}bird;

typedef struct{
    float maxJumpHeight;
    float airtimeFrames;
    float platformTravel;
    float minGap;
    float maxGap;
    float minPlatformLength;
}jumpMetrics;

class Button{
    public:
        Button(const char *normalPath, const char *hoverPath, const char *clickedPath, Vector2 position, float scale);
        ~Button();
        void Update();
        void Draw();
        bool isClicked();

    private:
        Texture2D normalTexture;
        Texture2D hoverTexture;
        Texture2D clickedTexture;
        Texture2D *currentTexture;
        
        Vector2 btnPos;
        Rectangle bounds;
        bool clicked;
};

void initGame(player *ply, platform p[], bird birds[], int screenWidth,float *speed, float *gaps, int *score);
void platformUpdate(float *speed, platform p[], bird birds[], float gaps, int *score);
void playerUpdate(player *ply);
void checkCollision(player *ply, platform p[]);
void checkGameOver(player *ply, bird birds[], int screenHeight, bool *gameOver);
int LoadHighScore(void);
void SaveHighScore(int hiScore);
jumpMetrics getJumpMetrics(float speed, float verticalDelta);
void generatePlatformAfter(platform *platformToPlace, float previousRight, float previousY, float speed, int score, bird birds[]);
void updateBirds(bird birds[], float speed);
void drawPlatforms(platform p[], Texture2D buildTexture);
void drawBirds(bird birds[], Texture2D birdTexture);
void clearBirds(bird birds[]);
void trySpawnBirdOnPlatform(const platform *plat, float gapBefore, float maxSafeGap, float speed, int score, bird birds[]);


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


//MAIN GAME WINDOW--------------------------------------------------------------------------------------------------------------------------
    while(!WindowShouldClose()){
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
                initGame(&ply,p,birds,screenWidth,&speed,&gaps,&score);
                scoreTimer = 0.0f;
                score = 0;
                gameOver = false;
                countdown = 3.0f;
                currentScreen = Scene1;
            }
            else if(quit1Button.isClicked()){
                CloseWindow();
            }
        }
        else if(currentScreen == Scene1){
            if(IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                currentScreen = Scene2;
            }
        }
        else if(currentScreen == Scene2){
            if(IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                currentScreen = Timer;
            }
        }
        else if(currentScreen == Timer){
            countdown -= GetFrameTime();
            if (countdown <0.0f){
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
                platformUpdate(&speed,p,birds,gaps,&score);
                updateBirds(birds, speed);
                playerUpdate(&ply);
                checkCollision(&ply,p);
                checkGameOver(&ply, birds, screenHeight, &gameOver);

                if(IsKeyPressed(KEY_P)){
                    currentScreen = Pause;
                }
            }
            else{
                if(score > hiScore){
                    hiScore = score;
                    SaveHighScore(hiScore);
                }
                
                if(IsKeyPressed(KEY_ENTER)){
                    initGame(&ply,p,birds,screenWidth,&speed,&gaps,&score);
                    scoreTimer = 0.0f;
                    score = 0;
                    countdown = 3.0f;
                    gameOver = false;
                }
            }
        }
        else if(currentScreen == Pause){
            
            if(IsKeyPressed(KEY_P)){
                currentScreen = Gameplay;
            }

            resumeButton.Update();
            homeButton.Update();
            quit2Button.Update();

            if(resumeButton.isClicked()){
                currentScreen = Gameplay;
            }
            else if(homeButton.isClicked()){
                currentScreen = Title;
            }
            else if(quit2Button.isClicked()){
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
                        DrawText("PRESS [ENTER] TO PLAY AGAIN", GetScreenWidth()/2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN",20)/2, GetScreenHeight()/2 - 20, 20, GRAY);
                }

                if(currentScreen == Pause){
                    DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.5f));
                    DrawRectangle(260,100,280,250,BLACK);
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

CloseWindow();
return 0;
}

//-----------------------------------------------------------------------------------------------------------------
// SCORE AND HIGH-SCORE SAVE
//-----------------------------------------------------------------------------------------------------------------
int LoadHighScore(void){
    int hi = 0;

    FILE *file = fopen("highscore.txt","r");
    if(file!=NULL){
        fscanf(file,"%d",&hi);
        fclose(file);
    }
    return hi;
}

void SaveHighScore(int hiScore){
    FILE *file = fopen("highscore.txt","w");
    if(file!=NULL){
        fprintf(file,"%d",hiScore);
        fclose(file);
    }
}

//-----------------------------------------------------------------------------------------------------------------
// GAME INIT
//-----------------------------------------------------------------------------------------------------------------
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
    
//-----------------------------------------------------------------------------------------------------------------
// PLATFORM UPDATE
//-----------------------------------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------------------------------
// PLAYER UPDATE
//-----------------------------------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------------------------------
// COLLISION CHECK
//-----------------------------------------------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------------------------------------------
// FAIR PLATFORM AND BIRD GENERATION
//-----------------------------------------------------------------------------------------------------------------
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

void clearBirds(bird birds[]){
    for(int i=0; i<MAX_BIRDS; i++){
        birds[i].active = false;
        birds[i].position = {-200.0f, -200.0f};
        birds[i].size = {140.0f, 94.0f};
        birds[i].height = BirdHigh;
    }
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

void drawPlatforms(platform p[], Texture2D buildTexture){
    Rectangle source = {0.0f, 0.0f, (float)buildTexture.width, (float)buildTexture.height};
    float platformLift = 12.0f;

    for(int i=0; i<MAX_P ; i++){
        Rectangle dest = {p[i].position.x, p[i].position.y - platformLift, p[i].size.x, p[i].size.y + platformLift};
        DrawTexturePro(buildTexture, source, dest, {0.0f, 0.0f}, 0.0f, WHITE);
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

//-----------------------------------------------------------------------------------------------------------------
// GAMEOVER
//-----------------------------------------------------------------------------------------------------------------
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


//-----------------------------------------------------------------------------------------------------------------
// Buttons
//-----------------------------------------------------------------------------------------------------------------
static Texture2D scaleTexture(const char *path, float scale){
    Image image = LoadImage(path);

    int newWidth = (int)(image.width * scale);
    int newHeight = (int)(image.height * scale);

    ImageResize(&image, newWidth, newHeight);
    Texture2D texture = LoadTextureFromImage(image);

    UnloadImage(image);

    return texture;
}

Button::Button(const char *normalPath, const char *hoverPath, const char *clickedPath, Vector2 position, float scale){
    normalTexture = scaleTexture(normalPath, scale);
    hoverTexture = scaleTexture(hoverPath, scale);
    clickedTexture = scaleTexture(clickedPath, scale);

    btnPos = position;
    currentTexture = &normalTexture;
    bounds = {btnPos.x, btnPos.y, (float)normalTexture.width, (float)normalTexture.height};
    clicked = false;
}

Button::~Button(){
    UnloadTexture(normalTexture);
    UnloadTexture(hoverTexture);
    UnloadTexture(clickedTexture);
}

void Button::Update(){
    clicked = false;
    Vector2 mousePos = GetMousePosition();

    if(CheckCollisionPointRec(mousePos, bounds)){
        if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
            currentTexture = &clickedTexture;
        }
        else{
            currentTexture = &hoverTexture;
        }

        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
            clicked = true;
        }
    }
    else{
        currentTexture = &normalTexture;
    }
}

void Button::Draw(){
    DrawTextureV(*currentTexture, btnPos, WHITE);
}

bool Button::isClicked(){
    return clicked;
}
