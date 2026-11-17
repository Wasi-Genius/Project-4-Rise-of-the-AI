#ifndef LEVEL_B_H
#define LEVEL_B_H

#include "Scene.h"

class LevelB : public Scene
{
private:
    // Warrior animation states
    enum WarriorAnimation
    {
        WARRIOR_IDLE,
        WARRIOR_RUN,
        WARRIOR_JUMP,
        WARRIOR_FALL,
        WARRIOR_ATTACK_ONE,
        WARRIOR_ATTACK_TWO
    };

    static constexpr int LEVEL_B_MAP_COLUMNS = 110,
                         LEVEL_B_MAP_ROWS = 20;
    static constexpr float LEVEL_B_TILE_SIZE = 32.0f;

    unsigned int mLevelData[
        LEVEL_B_MAP_COLUMNS * LEVEL_B_MAP_ROWS
    ] = {};

    // Level and character textures
    Texture2D mBackground{};
    Texture2D mHealthBar{};
    Texture2D mBlueBirch{};
    Texture2D mGreenBirch{};
    Texture2D mRedBirch{};
    Texture2D mSmallTent{};
    Texture2D mAngelStatue{};
    Texture2D mDecorAtlas{};
    Texture2D mWarriorIdle{};
    Texture2D mWarriorRun{};
    Texture2D mWarriorJump{};
    Texture2D mWarriorFall{};
    Texture2D mWarriorAttackOne{};
    Texture2D mWarriorAttackTwo{};
    // Enemy and level positions
    Entity *mWarrior = nullptr;
    Entity mLevelExit;
    Vector2 mPlayerStartPosition{};
    Vector2 mWarriorStartPosition{};
    WarriorAnimation mWarriorAnimation = WARRIOR_IDLE;
    int mWarriorAnimationFrame = 0;
    float mWarriorAnimationTime = 0.0f;
    float mWarriorJumpTime = 0.0f;
    bool mWarriorAnimationFinished = false;
    bool mWarriorDamageApplied = false;
    bool mWarriorAttackTwoNext = false;

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
    void updateWarrior(float deltaTime);
    void catchUpWarriorIfOffScreen();
    bool hasWarriorJumpClearance() const;
    void setWarriorAnimation(WarriorAnimation animation);
    WarriorAnimation getWarriorMovementAnimation() const;
    Texture2D getWarriorAnimationTexture() const;
    void animateWarrior(float deltaTime);
    void renderWarrior();
    void renderHealthBar();
};

#endif // LEVEL_B_H
