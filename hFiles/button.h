#pragma once
#define BUTTON_H
#include <raylib.h>



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