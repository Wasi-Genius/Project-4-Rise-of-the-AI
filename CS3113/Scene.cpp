#include "Scene.h"

GameProgress gGameProgress;

// The no-argument constructor provides a safe default for scenes that do not
// need custom world coordinates or a background colour.
Scene::Scene()
    : mGameState{}, mOrigin{ 0.0f, 0.0f },
      mBGColourHexCode{ "#000000" } {}

// Child scenes receive their own origin and background settings at creation.
Scene::Scene(Vector2 origin, const char *bgColourHexCode)
    : mGameState{}, mOrigin{ origin },
      mBGColourHexCode{ bgColourHexCode } {}
