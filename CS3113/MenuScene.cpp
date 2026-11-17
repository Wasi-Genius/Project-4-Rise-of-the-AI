#include "MenuScene.h"

MenuScene::MenuScene(Vector2 origin, const char *bgColourHexCode)
: Scene{ origin, bgColourHexCode } {}

MenuScene::~MenuScene()
{
    shutdown();
}

void MenuScene::initialise()
{
    // Start the menu music
    mGameState.bgm = LoadMusicStream("assets/BGM Music/Level 1 BGM.mp3");
    SetMusicVolume(mGameState.bgm, 0.5f);
    PlayMusicStream(mGameState.bgm);
    mGameState.nextSceneID = NO_SCENE;
}

void MenuScene::update(float)
{
    // Keep the music playing
    UpdateMusicStream(mGameState.bgm);
}

void MenuScene::render()
{
    // Show the game title and start prompt
    ClearBackground(ColorFromHex(mBGColourHexCode));
    DrawText("Knight's Run", 190, 220, 90, WHITE);
    DrawText("Press ENTER to start", 190, 350, 30, WHITE);
}

void MenuScene::shutdown()
{
    // Release the menu music
    UnloadMusicStream(mGameState.bgm);
    mGameState.bgm = {};
    mGameState.nextSceneID = NO_SCENE;
}
