#include"SpriteData.h"

GameLib::LoadTexture loadTexture[] = {
    {TEXNO::BACK00,L"./Data/Images/Back00.png",4U},
    {TEXNO::SPR000,L"./Data/Images/SPR000.png",1024U},
    {-1,nullptr},//èIóπÉtÉâÉO
};



SpriteUpperLeft sprBack(TEXNO::BACK00, 0, 0, 1280, 720);
SpriteCenter sprPlayer(TEXNO::SPR000, 256, 0, 128, 64);
SpriteCenter sprEnemy(TEXNO::SPR000, 192, 192, 64, 64);
