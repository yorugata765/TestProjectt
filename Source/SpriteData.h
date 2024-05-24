#pragma once

#include"../GameLib/game_lib.h"
#include"../GameLib/obj2d_data.h"
#include "../GameLib/texture.h"



enum TEXNO
{
    BACK00,
    SPR000,
};



struct SpriteUpperLeft :public GameLib::SpriteData
{
    SpriteUpperLeft(int texno, float left, float top, float width, float height)
    {
        this->texNo = texno;
        this->left = left;
        this->top = top;
        this->width = width;
        this->height = height;
        this->centerX = 0;
        this->centerY = 0;
    }
};

struct SpriteCenter :public GameLib::SpriteData
{

    SpriteCenter(int texno, float left, float top, float width, float height)
    {
        this->texNo = texno;
        this->left = left;
        this->top = top;
        this->width = width;
        this->height = height;
        this->centerX = width * 0.5f;
        this->centerY = height * 0.5f;
    }
};

extern GameLib::LoadTexture loadTexture[];

extern SpriteUpperLeft sprBack;
extern SpriteCenter sprPlayer;
extern SpriteCenter sprEnemy;




