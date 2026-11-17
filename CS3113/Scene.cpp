#include "Scene.h"

GameProgress gGameProgress;

// Set default scene coordinates and background color
Scene::Scene()
    : mGameState{}, mOrigin{ 0.0f, 0.0f },
      mBGColourHexCode{ "#000000" } {}

// Store the coordinates and background color for this scene
Scene::Scene(Vector2 origin, const char *bgColourHexCode)
    : mGameState{}, mOrigin{ origin },
      mBGColourHexCode{ bgColourHexCode } {}
