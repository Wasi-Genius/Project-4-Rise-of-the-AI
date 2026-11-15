#ifndef LEVEL_A_H
#define LEVEL_A_H

#include "Scene.h"

// LevelA is one concrete scene: its map data, assets, and transition request
// belong here rather than in the global game loop.
class LevelA : public Scene
{
private:
    static constexpr int LEVEL_A_MAP_COLUMNS = 84,
                         LEVEL_A_MAP_ROWS = 18;
    static constexpr float LEVEL_A_TILE_SIZE = 32.0f;

    unsigned int mLevelData[LEVEL_A_MAP_COLUMNS * LEVEL_A_MAP_ROWS] = {};

    Texture2D mBackground{};
    Texture2D mBlueBirch{};
    Texture2D mGreenBirch{};
    Texture2D mRedBirch{};
    Texture2D mSmallTent{};
    Texture2D mAngelStatue{};
    Vector2 mPlayerStartPosition{};

public:
    LevelA();
    LevelA(Vector2 origin, const char *bgColourHexCode);
    ~LevelA() override;

    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;

private:
    void buildLevelData();
};

#endif // LEVEL_A_H
