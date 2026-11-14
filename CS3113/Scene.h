#ifndef SCENE_H
#define SCENE_H

#include "Entity.h"

// These values describe the shared map and physics units used by both levels.
constexpr int SCENE_MAP_COLUMNS = 14,
              SCENE_MAP_ROWS = 8;

constexpr float SCENE_TILE_SIZE = 75.0f,
                SCENE_GRAVITY = 981.0f,
                SCENE_FALL_THRESHOLD = 800.0f;

// A scene groups resources that should exist only while that scene is active.
// The next ID is an enum so NO_SCENE cannot be confused with a real level.
struct GameState
{
    Entity *xochitl = nullptr;
    Map *map = nullptr;
    Music bgm{};
    Sound jumpSound{};
    SceneID nextSceneID = NO_SCENE;
};

// Every scene must load, update, draw, and release its own resources.
// Pure virtual methods make Scene an interface-like abstract base class.
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

    // Return a read-only reference so callers can inspect scene state without
    // copying the owning pointers inside GameState.
    const GameState &getState() const { return mGameState; }
    Vector2 getOrigin() const { return mOrigin; }
    const char *getBGColourHexCode() const { return mBGColourHexCode; }
};

#endif // SCENE_H
