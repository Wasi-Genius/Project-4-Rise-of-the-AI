#ifndef MENU_SCENE_H
#define MENU_SCENE_H

#include "Scene.h"

class MenuScene : public Scene
{
public:
    MenuScene(Vector2 origin, const char *bgColourHexCode);
    ~MenuScene() override;

    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};

#endif // MENU_SCENE_H
