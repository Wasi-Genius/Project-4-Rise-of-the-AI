#include "LoseScene.h"

LoseScene::LoseScene(Vector2 origin, const char *bgColourHexCode)
    : Scene{ origin, bgColourHexCode } {}

LoseScene::~LoseScene()
{
    shutdown();
}

void LoseScene::initialise()
{
    mLoseSound = LoadSound("assets/Misc. Sound/Lose Sound.mp3");
    PlaySound(mLoseSound);
    mGameState.nextSceneID = NO_SCENE;
}

void LoseScene::update(float) {}

void LoseScene::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));
    DrawText("You Lose", 190, 220, 90, WHITE);
    DrawText("Press ENTER to replay", 190, 350, 30, WHITE);
}

void LoseScene::shutdown()
{
    UnloadSound(mLoseSound);
    mLoseSound = {};
    mGameState.nextSceneID = NO_SCENE;
}
