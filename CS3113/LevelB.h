#ifndef LEVEL_B_H
#define LEVEL_B_H

#include "Scene.h"

// LevelB is a separate scene with its own GameState and map storage. Falling
// from it transitions back to LevelA to demonstrate switching in both directions.
class LevelB : public Scene
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
    LevelB();
    LevelB(Vector2 origin, const char *bgColourHexCode);
    ~LevelB() override;

    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};

#endif // LEVEL_B_H
