#include"Obj2d.h"
#include "SpriteData.h"



void OBJ2D::draw()
{
    auto prevState = GameLib::getBlendMode();
    GameLib::setBlendMode(blendState);
    if (sprData) (sprData->draw(position, scale, rotation, color));
    GameLib::setBlendMode(prevState);
}
