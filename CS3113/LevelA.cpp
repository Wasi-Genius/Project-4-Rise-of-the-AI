#include "LevelA.h"

// The default and configured constructors pass this scene's coordinate
// origin and background colour to the shared Scene base.
LevelA::LevelA() : Scene{ { 0.0f, 0.0f }, "#000000" } {}

LevelA::LevelA(Vector2 origin, const char *bgColourHexCode)
    : Scene{ origin, bgColourHexCode } {}

LevelA::~LevelA()
{
    shutdown();
}

void LevelA::initialise()
{
    // A scene loads its own audio and map so another level can use different
    // assets without changing the main game loop.
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

    // Each direction maps to frame indices in the character sprite atlas.
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

    // Gravity is acceleration in pixels per second squared; the entity uses
    // this with the fixed timestep supplied by the game loop.
    mGameState.xochitl->setJumpingPower(550.0f);
    mGameState.xochitl->setColliderDimensions({
        mGameState.xochitl->getScale().x / 3.5f,
        mGameState.xochitl->getScale().y / 3.0f
    });
    mGameState.xochitl->setAcceleration({ 0.0f, SCENE_GRAVITY });
}

void LevelA::update(float deltaTime)
{
    UpdateMusicStream(mGameState.bgm);
    mGameState.xochitl->update(
        deltaTime,
        nullptr,
        mGameState.map,
        nullptr,
        0
    );

    // The scene requests a transition; main performs the actual scene switch.
    if (mGameState.xochitl->getPosition().y > SCENE_FALL_THRESHOLD)
        mGameState.nextSceneID = LEVEL_B;
}

void LevelA::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));
    mGameState.map->render();
    mGameState.xochitl->render();
}

void LevelA::shutdown()
{
    // Clearing each resource after releasing it makes this safe to call when
    // switching scenes and again from the destructor.
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
