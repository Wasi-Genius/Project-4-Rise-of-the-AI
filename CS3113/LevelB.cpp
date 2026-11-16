#include "LevelB.h"

namespace
{
void addPlatform(
    unsigned int *levelData,
    int mapColumns,
    int mapRows,
    int startColumn,
    int endColumn,
    int topRow,
    int fillRows
)
{
    for (int column = startColumn; column <= endColumn; column++)
    {
        unsigned int topTile = 110;
        if (column == startColumn) topTile = 109;
        else if (column == endColumn) topTile = 111;

        levelData[topRow * mapColumns + column] = topTile;

        for (int depth = 1; depth <= fillRows && topRow + depth < mapRows;
             depth++)
            levelData[(topRow + depth) * mapColumns + column] = 119;
    }
}

void drawTexture(Texture2D texture, Rectangle destination)
{
    DrawTexturePro(
        texture,
        { 0.0f, 0.0f, static_cast<float>(texture.width),
          static_cast<float>(texture.height) },
        destination,
        { 0.0f, 0.0f },
        0.0f,
        WHITE
    );
}

void drawAtGround(Texture2D texture, float centerX, float groundY, float scale)
{
    const float width = texture.width * scale;
    const float height = texture.height * scale;
    drawTexture(texture, {
        centerX - width / 2.0f,
        groundY - height,
        width,
        height
    });
}

void drawAtlasItemAtGround(
    Texture2D texture,
    Rectangle source,
    float centerX,
    float groundY,
    float scale
)
{
    const float width = source.width * scale;
    const float height = source.height * scale;
    DrawTexturePro(
        texture,
        source,
        { centerX - width / 2.0f, groundY - height, width, height },
        { 0.0f, 0.0f },
        0.0f,
        WHITE
    );
}
}

LevelB::LevelB() : Scene{ { 0.0f, 0.0f }, "#000000" } {}

LevelB::LevelB(Vector2 origin, const char *bgColourHexCode)
    : Scene{ origin, bgColourHexCode } {}

LevelB::~LevelB()
{
    shutdown();
}

void LevelB::initialise()
{
    mGameState.bgm = LoadMusicStream("assets/BGM Music/Level 2 BGM.mp3");
    SetMusicVolume(mGameState.bgm, 0.5f);
    PlayMusicStream(mGameState.bgm);
    mGameState.jumpSound = LoadSound("assets/Misc. Sound/Jump Sound.mp3");
    mGameState.nextSceneID = NO_SCENE;

    mBackground = LoadTexture(
        "assets/Tiles and Background/Purple Background.png"
    );
    mHealthBar = LoadTexture("assets/Health Bar.png");
    mBlueBirch = LoadTexture("assets/Tiles and Background/Blue Birch.png");
    mGreenBirch = LoadTexture("assets/Tiles and Background/Green Birch.png");
    mRedBirch = LoadTexture("assets/Tiles and Background/Red Birch.png");
    mSmallTent = LoadTexture("assets/Tiles and Background/Small Tent.png");
    mAngelStatue = LoadTexture("assets/Tiles and Background/Angel Statue.png");
    mDecorAtlas = LoadTexture("assets/Tiles and Background/Decor.png");

    buildLevelData();
    mGameState.map = new Map(
        LEVEL_B_MAP_COLUMNS,
        LEVEL_B_MAP_ROWS,
        mLevelData,
        "assets/Tiles and Background/Floor Tiles.png",
        LEVEL_B_TILE_SIZE,
        9,
        18,
        mOrigin
    );

    const std::vector<int> runningFrames = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9
    };
    const std::map<Direction, std::vector<int>> animationAtlas = {
        { DOWN, runningFrames },
        { LEFT, runningFrames },
        { UP, runningFrames },
        { RIGHT, runningFrames }
    };

    mGameState.player = new Entity(
        { 0.0f, 0.0f },
        { 120.0f, 80.0f },
        "assets/Protag/_Run.png",
        ATLAS,
        { 1, 10 },
        animationAtlas,
        PLAYER
    );
    mGameState.player->setTurnAroundAnimation(
        "assets/Protag/_TurnAround.png",
        { 1, 3 },
        { 0, 1, 2 }
    );

    const float groundY = mGameState.map->getTopBoundary() +
        17.0f * LEVEL_B_TILE_SIZE;
    mPlayerStartPosition = {
        mGameState.map->getLeftBoundary() + 4.0f * LEVEL_B_TILE_SIZE,
        groundY - mGameState.player->getScale().y / 2.0f
    };
    mGameState.player->setPosition(mPlayerStartPosition);
    mGameState.player->setJumpingPower(520.0f);
    mGameState.player->setColliderDimensions({ 30.0f, 40.0f });
    mGameState.player->setColliderOffset({ -4.0f, 20.0f });
    mGameState.player->setAcceleration({ 0.0f, SCENE_GRAVITY });
}

void LevelB::buildLevelData()
{
    addPlatform(
        mLevelData, LEVEL_B_MAP_COLUMNS, LEVEL_B_MAP_ROWS, 0, 19, 17, 2
    );
    addPlatform(
        mLevelData, LEVEL_B_MAP_COLUMNS, LEVEL_B_MAP_ROWS, 22, 40, 17, 2
    );
    addPlatform(
        mLevelData, LEVEL_B_MAP_COLUMNS, LEVEL_B_MAP_ROWS, 43, 61, 17, 2
    );
    addPlatform(
        mLevelData, LEVEL_B_MAP_COLUMNS, LEVEL_B_MAP_ROWS, 65, 82, 17, 2
    );
    addPlatform(
        mLevelData, LEVEL_B_MAP_COLUMNS, LEVEL_B_MAP_ROWS, 85, 109, 17, 2
    );

    addPlatform(
        mLevelData, LEVEL_B_MAP_COLUMNS, LEVEL_B_MAP_ROWS, 8, 12, 14, 0
    );
    addPlatform(
        mLevelData, LEVEL_B_MAP_COLUMNS, LEVEL_B_MAP_ROWS, 14, 18, 12, 0
    );
    addPlatform(
        mLevelData, LEVEL_B_MAP_COLUMNS, LEVEL_B_MAP_ROWS, 25, 30, 14, 0
    );
    addPlatform(
        mLevelData, LEVEL_B_MAP_COLUMNS, LEVEL_B_MAP_ROWS, 32, 36, 11, 0
    );
    addPlatform(
        mLevelData, LEVEL_B_MAP_COLUMNS, LEVEL_B_MAP_ROWS, 40, 45, 14, 0
    );
    addPlatform(
        mLevelData, LEVEL_B_MAP_COLUMNS, LEVEL_B_MAP_ROWS, 49, 54, 12, 0
    );
    addPlatform(
        mLevelData, LEVEL_B_MAP_COLUMNS, LEVEL_B_MAP_ROWS, 57, 62, 9, 0
    );
    addPlatform(
        mLevelData, LEVEL_B_MAP_COLUMNS, LEVEL_B_MAP_ROWS, 67, 72, 13, 0
    );
    addPlatform(
        mLevelData, LEVEL_B_MAP_COLUMNS, LEVEL_B_MAP_ROWS, 75, 80, 11, 0
    );
    addPlatform(
        mLevelData, LEVEL_B_MAP_COLUMNS, LEVEL_B_MAP_ROWS, 84, 89, 14, 0
    );
    addPlatform(
        mLevelData, LEVEL_B_MAP_COLUMNS, LEVEL_B_MAP_ROWS, 93, 98, 12, 0
    );
    addPlatform(
        mLevelData, LEVEL_B_MAP_COLUMNS, LEVEL_B_MAP_ROWS, 101, 106, 10, 0
    );
}

void LevelB::update(float deltaTime)
{
    UpdateMusicStream(mGameState.bgm);
    mGameState.player->update(
        deltaTime,
        nullptr,
        mGameState.map,
        nullptr,
        0
    );

    if (mGameState.player->getPosition().y > SCENE_FALL_THRESHOLD)
    {
        if (gGameProgress.lives > 0)
        {
            gGameProgress.lives--;
            PlaySound(gGameProgress.hurtSound);
        }

        if (gGameProgress.lives <= 0)
        {
            mGameState.nextSceneID = LOSE_SCREEN;
            return;
        }

        mGameState.player->setPosition(mPlayerStartPosition);
        mGameState.player->setVelocity({ 0.0f, 0.0f });
        mGameState.player->setMovement({ 0.0f, 0.0f });
    }
}

void LevelB::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));

    const Vector2 playerPosition = mGameState.player->getPosition();
    const float backgroundWidth = 2.0f * mOrigin.x;
    const float backgroundHeight = 2.0f * mOrigin.y;
    const float backgroundOffset = playerPosition.x * 0.2f;
    const float backgroundLeft =
        playerPosition.x - mOrigin.x - backgroundOffset;

    drawTexture(mBackground, {
        backgroundLeft - backgroundWidth, 0.0f,
        backgroundWidth, backgroundHeight
    });
    drawTexture(mBackground, {
        backgroundLeft, 0.0f,
        backgroundWidth, backgroundHeight
    });
    drawTexture(mBackground, {
        backgroundLeft + backgroundWidth, 0.0f,
        backgroundWidth, backgroundHeight
    });

    mGameState.map->render();

    const float mapLeft = mGameState.map->getLeftBoundary();
    const float groundY = mGameState.map->getTopBoundary() +
        17.0f * LEVEL_B_TILE_SIZE;
    drawAtGround(mBlueBirch, mapLeft + 5.0f * LEVEL_B_TILE_SIZE, groundY, 1.5f);
    drawAtGround(mRedBirch, mapLeft + 13.0f * LEVEL_B_TILE_SIZE, groundY, 1.5f);
    drawAtGround(mSmallTent, mapLeft + 20.0f * LEVEL_B_TILE_SIZE, groundY, 1.3f);
    drawAtGround(mGreenBirch, mapLeft + 28.0f * LEVEL_B_TILE_SIZE, groundY, 1.5f);
    drawAtlasItemAtGround(
        mDecorAtlas,
        { 192.0f, 98.0f, 164.0f, 48.0f },
        mapLeft + 39.0f * LEVEL_B_TILE_SIZE,
        groundY,
        1.0f
    );
    drawAtGround(mAngelStatue, mapLeft + 48.0f * LEVEL_B_TILE_SIZE, groundY, 1.3f);
    drawAtGround(mSmallTent, mapLeft + 57.0f * LEVEL_B_TILE_SIZE, groundY, 1.3f);
    drawAtlasItemAtGround(
        mDecorAtlas,
        { 315.0f, 134.0f, 48.0f, 42.0f },
        mapLeft + 64.0f * LEVEL_B_TILE_SIZE,
        groundY,
        1.4f
    );
    drawAtGround(mRedBirch, mapLeft + 73.0f * LEVEL_B_TILE_SIZE, groundY, 1.5f);
    drawAtlasItemAtGround(
        mDecorAtlas,
        { 0.0f, 235.0f, 238.0f, 60.0f },
        mapLeft + 86.0f * LEVEL_B_TILE_SIZE,
        groundY,
        0.75f
    );
    drawAtGround(mBlueBirch, mapLeft + 96.0f * LEVEL_B_TILE_SIZE, groundY, 1.5f);
    drawAtGround(mGreenBirch, mapLeft + 104.0f * LEVEL_B_TILE_SIZE, groundY, 1.5f);

    drawAtlasItemAtGround(
        mDecorAtlas,
        { 387.0f, 265.0f, 25.0f, 55.0f },
        mGameState.map->getRightBoundary() - LEVEL_B_TILE_SIZE,
        groundY,
        1.5f
    );

    mGameState.player->render();
    renderHealthBar();
}

void LevelB::renderHealthBar()
{
    const float screenLeft =
        mGameState.player->getPosition().x - mOrigin.x + 14.0f;
    const float screenTop = 12.0f;
    const float fillWidth = 82.0f * (gGameProgress.lives / 5.0f);

    DrawTexturePro(
        mHealthBar,
        { 42.0f, 42.0f, fillWidth, 14.0f },
        { screenLeft + 42.0f, screenTop + 13.0f, fillWidth, 14.0f },
        { 0.0f, 0.0f },
        0.0f,
        WHITE
    );
    DrawTexturePro(
        mHealthBar,
        { 0.0f, 0.0f, 128.0f, 36.0f },
        { screenLeft, screenTop, 128.0f, 36.0f },
        { 0.0f, 0.0f },
        0.0f,
        WHITE
    );
}

void LevelB::shutdown()
{
    delete mGameState.player;
    mGameState.player = nullptr;
    delete mGameState.map;
    mGameState.map = nullptr;

    UnloadTexture(mBackground);
    mBackground = {};
    UnloadTexture(mHealthBar);
    mHealthBar = {};
    UnloadTexture(mBlueBirch);
    mBlueBirch = {};
    UnloadTexture(mGreenBirch);
    mGreenBirch = {};
    UnloadTexture(mRedBirch);
    mRedBirch = {};
    UnloadTexture(mSmallTent);
    mSmallTent = {};
    UnloadTexture(mAngelStatue);
    mAngelStatue = {};
    UnloadTexture(mDecorAtlas);
    mDecorAtlas = {};

    UnloadMusicStream(mGameState.bgm);
    mGameState.bgm = {};
    UnloadSound(mGameState.jumpSound);
    mGameState.jumpSound = {};
    mGameState.nextSceneID = NO_SCENE;
}
