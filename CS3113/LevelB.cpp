#include "LevelB.h"

// LevelB shares the same scene contract, but has its own background and
// resources. Keeping it as a different type makes scene-specific behavior clear.
LevelB::LevelB() : Scene{ { 0.0f, 0.0f }, "#000000" } {}

LevelB::LevelB(Vector2 origin, const char *bgColourHexCode)
    : Scene{ origin, bgColourHexCode } {}

LevelB::~LevelB()
{
    shutdown();
}

void LevelB::initialise()
{
    mGameState.bgm = LoadMusicStream("assets/game/04 - Silent Forest.wav");
    SetMusicVolume(mGameState.bgm, 0.33f);
    PlayMusicStream(mGameState.bgm);
    mGameState.jumpSound = LoadSound("assets/game/Dirt Jump.wav");
    mGameState.nextSceneID = NO_SCENE;

    mGameState.map = new Map(
        SCENE_MAP_COLUMNS,
        SCENE_MAP_ROWS,
        mLevelData,
        "assets/game/tileset.png",
        SCENE_TILE_SIZE,
        4,
        1,
        mOrigin
    );

    std::map<Direction, std::vector<int>> animationAtlas = {
        { DOWN,  {  0,  1,  2,  3,  4,  5,  6,  7 } },
        { LEFT,  {  8,  9, 10, 11, 12, 13, 14, 15 } },
        { UP,    { 24, 25, 26, 27, 28, 29, 30, 31 } },
        { RIGHT, { 40, 41, 42, 43, 44, 45, 46, 47 } }
    };

    const float sizeRatio = 48.0f / 64.0f;
    mGameState.xochitl = new Entity(
        { mOrigin.x - 300.0f, mOrigin.y - 200.0f },
        { 250.0f * sizeRatio, 250.0f },
        "assets/game/walk.png",
        ATLAS,
        { 6, 8 },
        animationAtlas,
        PLAYER
    );
    mGameState.xochitl->setJumpingPower(550.0f);
    mGameState.xochitl->setColliderDimensions({
        mGameState.xochitl->getScale().x / 3.5f,
        mGameState.xochitl->getScale().y / 3.0f
    });
    mGameState.xochitl->setAcceleration({ 0.0f, SCENE_GRAVITY });
}

void LevelB::update(float deltaTime)
{
    UpdateMusicStream(mGameState.bgm);
    mGameState.xochitl->update(
        deltaTime,
        nullptr,
        mGameState.map,
        nullptr,
        0
    );

    // LEVEL_A has value zero and is still a valid destination because the
    // distinct NO_SCENE value is -1 rather than zero.
    if (mGameState.xochitl->getPosition().y > SCENE_FALL_THRESHOLD)
        mGameState.nextSceneID = LEVEL_A;
}

void LevelB::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));
    mGameState.map->render();
    mGameState.xochitl->render();
}

void LevelB::shutdown()
{
    delete mGameState.xochitl;
    mGameState.xochitl = nullptr;
    delete mGameState.map;
    mGameState.map = nullptr;

    UnloadMusicStream(mGameState.bgm);
    mGameState.bgm = {};
    UnloadSound(mGameState.jumpSound);
    mGameState.jumpSound = {};
    mGameState.nextSceneID = NO_SCENE;
}
