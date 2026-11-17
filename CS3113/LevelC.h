#ifndef LEVEL_C_H
#define LEVEL_C_H

#include "Scene.h"

class LevelC : public Scene
{
private:
    enum WizardAnimation
    {
        WIZARD_IDLE,
        WIZARD_RUN,
        WIZARD_JUMP,
        WIZARD_FALL,
        WIZARD_ATTACK_ONE,
        WIZARD_ATTACK_TWO
    };

    // Each map cell is 32px; row 18 is the main floor.
    static constexpr int LEVEL_C_MAP_COLUMNS = 124,
                         LEVEL_C_MAP_ROWS = 22;
    static constexpr float LEVEL_C_TILE_SIZE = 32.0f;

    // Zero cells are empty; nonzero values are Floor Tiles atlas IDs.
    unsigned int mLevelData[
        LEVEL_C_MAP_COLUMNS * LEVEL_C_MAP_ROWS
    ] = {};
    // Separate spike flags let solid map cells render with the right orientation.
    bool mSpikeTiles[
        LEVEL_C_MAP_COLUMNS * LEVEL_C_MAP_ROWS
    ] = {};
    bool mGroundSpikeTiles[
        LEVEL_C_MAP_COLUMNS * LEVEL_C_MAP_ROWS
    ] = {};

    // Textures loaded and released by this scene.
    Texture2D mBackground{};
    Texture2D mHealthBar{};
    Texture2D mAngelStatue{};
    Texture2D mDecorAtlas{};
    Texture2D mWizardIdle{};
    Texture2D mWizardRun{};
    Texture2D mWizardJump{};
    Texture2D mWizardFall{};
    Texture2D mWizardAttackOne{};
    Texture2D mWizardAttackTwo{};
    Entity mLevelExit;
    Entity *mWizard = nullptr;
    Vector2 mPlayerStartPosition{};
    // Prevent repeated damage while the player remains on one spike contact.
    bool mPlayerOnSpikes = false;
    WizardAnimation mWizardAnimation = WIZARD_IDLE;
    int mWizardAnimationFrame = 0;
    float mWizardAnimationTime = 0.0f;
    bool mWizardAnimationFinished = false;
    bool mWizardAttackTwoNext = false;
    bool mWizardWasTouchingPlayer = false;

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
    void updateWizard(float deltaTime);
    void setWizardAnimation(WizardAnimation animation);
    WizardAnimation getWizardMovementAnimation() const;
    Texture2D getWizardAnimationTexture() const;
    void animateWizard(float deltaTime);
    void renderWizard();
    float getGroundYAtColumn(int column) const;
    void drawDecorationAtGround(
        Texture2D texture,
        Rectangle source,
        float tileColumn,
        float scale);
    void renderSpikes();
    void renderHealthBar();
};

#endif // LEVEL_C_H
