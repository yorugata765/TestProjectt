#pragma once
#include"../GameLib/game_lib.h"
#include"../GameLib/obj2d_data.h"

class OBJ2D
{
public:
    GameLib::SpriteData* sprData;

    VECTOR2 position;

    OBJ2D()
        :sprData(nullptr)
        , position({ 0,0 })
        , scale({ 1.0f,1.0f })
        , rotation(0.0f)
        , color({ 1,1,1,1 })
        , blendState(GameLib::Blender::BLEND_STATE::BS_ALPHA) {}


    void draw();

    VECTOR2 scale;
    float rotation;
    VECTOR4 color;
    GameLib::Blender::BLEND_STATE blendState;



};
