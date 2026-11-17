#include "LoseScene.h"

LoseScene::LoseScene(Vector2 origin, const char *bgColourHexCode)
    : Scene{ origin, bgColourHexCode } {}

LoseScene::~LoseScene()
{
    shutdown();
}

void LoseScene::initialise()
{
    // Play the lose sound when the scene opens
    mLoseSound = LoadSound("assets/Misc. Sound/Lose Sound.mp3");
    PlaySound(mLoseSound);
    mGameState.nextSceneID = NO_SCENE;
}

void LoseScene::update(float) {}

void LoseScene::render()
{
    // Show the replay prompt
    ClearBackground(ColorFromHex(mBGColourHexCode));
    DrawText("You Lose", 190, 220, 90, WHITE);
    DrawText("Press ENTER to replay", 190, 350, 30, WHITE);
}

void LoseScene::shutdown()
{
    // Release the lose sound
    UnloadSound(mLoseSound);
    mLoseSound = {};
    mGameState.nextSceneID = NO_SCENE;
}
