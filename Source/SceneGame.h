#pragma once
#include "Scene.h"

class SceneGame : public Scene
{
private:
    void init() override;
    void deinit()override;
    void update() override;
    void draw() override;
};
