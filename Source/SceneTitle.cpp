#include "SceneTitle.h"
#include "../GameLib/game_lib.h"
#include "WinMain.h"

void SceneTitle::init()
{
    timer = 0;
}

void SceneTitle::update()
{
    using namespace GameLib::input;
    if (TRG(0) & PAD_START)
    {
        setScene(SCENE::GAME);
    }

    timer++;
}

void SceneTitle::draw()
{
    GameLib::clear(0, 0, 0);

    GameLib::font::textOut(4, "C++ Shooting", { 640, 360 }, { 5, 5 },
        { 1, 1, 1, 1 }, GameLib::TEXT_ALIGN::MIDDLE);

    if (timer / 30 % 2)
    {
        GameLib::font::textOut(4, "Push space key", { 640, 540 }, { 2, 2 },
            { 1, 1, 0, 1 }, GameLib::TEXT_ALIGN::MIDDLE);
    }
}
