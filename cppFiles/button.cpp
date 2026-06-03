#include "../hFiles/button.h"

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
