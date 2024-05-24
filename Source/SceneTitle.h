#pragma once
#include "Scene.h"

class SceneTitle : public Scene
{
private:
    void init() override;
    void update() override;
    void draw() override;
};
