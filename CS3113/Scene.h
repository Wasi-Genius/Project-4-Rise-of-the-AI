#ifndef SCENE_H
#define SCENE_H

#include "Entity.h"

// Shared map and physics values
constexpr int SCENE_MAP_COLUMNS = 14,
              SCENE_MAP_ROWS = 8;

constexpr float SCENE_TILE_SIZE = 75.0f,
                SCENE_GRAVITY = 981.0f,
                SCENE_FALL_THRESHOLD = 800.0f;

struct GameProgress
{
    int lives = 5;
    Sound hurtSound{};
};

extern GameProgress gGameProgress;

// State owned by the active scene
struct GameState
{
    Entity *player = nullptr;
    Map *map = nullptr;
    Music bgm{};
    Sound jumpSound{};
    SceneID nextSceneID = NO_SCENE;
};

// Each scene loads, updates, draws, and releases its own resources
class Scene
{
protected:
    GameState mGameState;
    Vector2 mOrigin;
    const char *mBGColourHexCode;

public:
    Scene();
    Scene(Vector2 origin, const char *bgColourHexCode);
    virtual ~Scene() {}

    virtual void initialise() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;
    virtual void shutdown() = 0;

    // Read scene state without copying its pointers
    const GameState &getState() const { return mGameState; }
    void requestSceneChange(SceneID sceneID) { mGameState.nextSceneID = sceneID; }
    Vector2 getOrigin() const { return mOrigin; }
    const char *getBGColourHexCode() const { return mBGColourHexCode; }
};

#endif // SCENE_H
