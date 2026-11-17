#ifndef LEVEL_A_H
#define LEVEL_A_H

#include "Scene.h"

// Level A owns its map, assets, and exit logic
class LevelA : public Scene
{
private:
    // Huntress animation states
    enum HuntressAnimation
    {
        HUNTRESS_IDLE,
        HUNTRESS_RUN,
        HUNTRESS_JUMP,
        HUNTRESS_FALL,
        HUNTRESS_ATTACK_ONE,
        HUNTRESS_ATTACK_TWO
    };

    static constexpr int LEVEL_A_MAP_COLUMNS = 84,
                         LEVEL_A_MAP_ROWS = 18;
    static constexpr float LEVEL_A_TILE_SIZE = 32.0f;

    unsigned int mLevelData[LEVEL_A_MAP_COLUMNS * LEVEL_A_MAP_ROWS] = {};

    // Level and character textures
    Texture2D mBackground{};
    Texture2D mHealthBar{};
    Texture2D mBlueBirch{};
    Texture2D mGreenBirch{};
    Texture2D mRedBirch{};
    Texture2D mSmallTent{};
    Texture2D mAngelStatue{};
    Texture2D mDecorAtlas{};
    Texture2D mHuntressIdle{};
    Texture2D mHuntressRun{};
    Texture2D mHuntressJump{};
    Texture2D mHuntressFall{};
    Texture2D mHuntressAttackOne{};
    Texture2D mHuntressAttackTwo{};
    // Enemies and level positions
    Entity *mHuntress = nullptr;
    Entity mLevelExit;
    Vector2 mPlayerStartPosition{};
    Vector2 mHuntressStartPosition{};
    HuntressAnimation mHuntressAnimation = HUNTRESS_IDLE;
    int mHuntressAnimationFrame = 0;
    float mHuntressAnimationTime = 0.0f;
    bool mHuntressAnimationFinished = false;
    bool mHuntressAttackTwoNext = false;
    bool mHuntressDamageApplied = false;

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
    void updateHuntress(float deltaTime);
    void setHuntressAnimation(HuntressAnimation animation);
    HuntressAnimation getHuntressMovementAnimation(bool isFollowing) const;
    void animateHuntress(float deltaTime);
    Texture2D getHuntressAnimationTexture() const;
    void renderHuntress();
    void renderHealthBar();
    void loseLife();
};

#endif // LEVEL_A_H
