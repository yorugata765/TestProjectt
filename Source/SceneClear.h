#pragma once
#include "Scene.h"

class SceneClear : public Scene
{
private:
    void init() override;
    void update() override;
    void draw() override;
};
