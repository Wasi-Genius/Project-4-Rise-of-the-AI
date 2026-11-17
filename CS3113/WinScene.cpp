#include "WinScene.h"

WinScene::WinScene(Vector2 origin, const char *bgColourHexCode)
    : Scene{origin, bgColourHexCode} {}

WinScene::~WinScene()
{
    shutdown();
}

void WinScene::initialise()
{
    // Play the victory sound when the scene opens
    mVictorySound = LoadSound("assets/Misc. Sound/Victory Sound.mp3");
    PlaySound(mVictorySound);
    mGameState.nextSceneID = NO_SCENE;
}

void WinScene::update(float) {}

void WinScene::render()
{
    // Show the replay prompt
    ClearBackground(ColorFromHex(mBGColourHexCode));
    DrawText("You Win", 190, 220, 90, WHITE);
    DrawText("Press ENTER to replay", 190, 350, 30, WHITE);
}

void WinScene::shutdown()
{
    // Release the victory sound
    UnloadSound(mVictorySound);
    mVictorySound = {};
    mGameState.nextSceneID = NO_SCENE;
}
