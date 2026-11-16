#ifndef LOSE_SCENE_H
#define LOSE_SCENE_H

#include "Scene.h"

class LoseScene : public Scene
{
private:
    Sound mLoseSound{};

public:
    LoseScene(Vector2 origin, const char *bgColourHexCode);
    ~LoseScene() override;

    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};

#endif // LOSE_SCENE_H
