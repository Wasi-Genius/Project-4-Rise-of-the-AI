#ifndef LEVEL_B_H
#define LEVEL_B_H

#include "Scene.h"

class LevelB : public Scene
{
private:
    static constexpr int LEVEL_B_MAP_COLUMNS = 110,
                         LEVEL_B_MAP_ROWS = 20;
    static constexpr float LEVEL_B_TILE_SIZE = 32.0f;

    unsigned int mLevelData[
        LEVEL_B_MAP_COLUMNS * LEVEL_B_MAP_ROWS
    ] = {};

    Texture2D mBackground{};
    Texture2D mHealthBar{};
    Texture2D mBlueBirch{};
    Texture2D mGreenBirch{};
    Texture2D mRedBirch{};
    Texture2D mSmallTent{};
    Texture2D mAngelStatue{};
    Texture2D mDecorAtlas{};
    Vector2 mPlayerStartPosition{};

public:
    LevelB();
    LevelB(Vector2 origin, const char *bgColourHexCode);
    ~LevelB() override;

    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;

private:
    void buildLevelData();
    void renderHealthBar();
};

#endif // LEVEL_B_H
