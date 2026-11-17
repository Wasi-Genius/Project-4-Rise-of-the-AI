#ifndef LEVEL_C_H
#define LEVEL_C_H

#include "Scene.h"

class LevelC : public Scene
{
private:
    // Each map cell is 32px; row 18 is the main floor.
    static constexpr int LEVEL_C_MAP_COLUMNS = 124,
                         LEVEL_C_MAP_ROWS = 22;
    static constexpr float LEVEL_C_TILE_SIZE = 32.0f;

    // Zero cells are empty; nonzero values are Floor Tiles atlas IDs.
    unsigned int mLevelData[
        LEVEL_C_MAP_COLUMNS * LEVEL_C_MAP_ROWS
    ] = {};

    // Textures loaded and released by this scene.
    Texture2D mBackground{};
    Texture2D mCaveDirt{}; // BG Dirt2, cropped in drawCaveDirtPatch().
    Texture2D mHealthBar{};
    Texture2D mAngelStatue{};
    Texture2D mDecorAtlas{};
    Vector2 mPlayerStartPosition{};
    // Prevent repeated damage while the player remains on one spike contact.
    bool mPlayerOnSpikes = false;

public:
    LevelC();
    LevelC(Vector2 origin, const char *bgColourHexCode);
    ~LevelC() override;

    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;

private:
    // Level layout, spike collision, shared life loss, and HUD drawing.
    void buildLevelData();
    bool isPlayerTouchingSpikes() const;
    void loseLife();
    void renderHealthBar();
};

#endif // LEVEL_C_H
