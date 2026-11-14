#ifndef LEVEL_A_H
#define LEVEL_A_H

#include "Scene.h"

// LevelA is one concrete scene: its map data, assets, and transition request
// belong here rather than in the global game loop.
class LevelA : public Scene
{
private:
    unsigned int mLevelData[SCENE_MAP_COLUMNS * SCENE_MAP_ROWS] = {
        4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4,
        4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4,
        4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4,
        4, 0, 0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 4,
        4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 4,
        4, 2, 2, 2, 0, 0, 0, 2, 2, 2, 3, 3, 3, 4,
        4, 3, 3, 3, 0, 0, 0, 3, 3, 3, 3, 3, 3, 4,
        4, 3, 3, 3, 0, 0, 0, 3, 3, 3, 3, 3, 3, 4
    };

public:
    LevelA();
    LevelA(Vector2 origin, const char *bgColourHexCode);
    ~LevelA() override;

    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};

#endif // LEVEL_A_H
