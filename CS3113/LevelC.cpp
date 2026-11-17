#include "LevelC.h"

namespace
{
    const float LEVEL_C_STATUE_SCALE = 1.3f;

    void addCavePlatform(
        unsigned int *levelData,
        int mapColumns,
        int mapRows,
        int startColumn,
        int endColumn,
        int topRow,
        int fillRows)
    {
        // Brown Floor Tiles IDs: 55-57 for platform tops.
        for (int column = startColumn; column <= endColumn; column++)
        {
            unsigned int topTile = 56;
            if (column == startColumn)
                topTile = 55;
            else if (column == endColumn)
                topTile = 57;

            levelData[topRow * mapColumns + column] = topTile;

            for (int depth = 1;
                 depth <= fillRows && topRow + depth < mapRows;
                 depth++)
            {
                levelData[(topRow + depth) * mapColumns + column] = 65;
            }
        }
    }

    void addSpikeRun(
        unsigned int *levelData,
        bool *spikeTiles,
        bool *groundSpikeTiles,
        int mapColumns,
        int row,
        int startColumn,
        int endColumn,
        bool pointsUp)
    {
        // Keep the map cell solid
        for (int column = startColumn; column <= endColumn; column++)
        {
            const int tileIndex = row * mapColumns + column;
            levelData[tileIndex] = 65;
            spikeTiles[tileIndex] = true;
            groundSpikeTiles[tileIndex] = pointsUp;
        }
    }

    void drawTexture(Texture2D texture, Rectangle destination)
    {
        DrawTexturePro(
            texture,
            {0.0f, 0.0f, static_cast<float>(texture.width),
             static_cast<float>(texture.height)},
            destination,
            {0.0f, 0.0f},
            0.0f,
            WHITE);
    }

    void drawAtlasItemAtGround(
        Texture2D texture,
        Rectangle source,
        float centerX,
        float groundY,
        float scale)
    {
        const float width = source.width * scale;
        const float height = source.height * scale;
        DrawTexturePro(
            texture,
            source,
            {centerX - width / 2.0f, groundY - height, width, height},
            {0.0f, 0.0f},
            0.0f,
            WHITE);
    }
}

LevelC::LevelC() : Scene{{0.0f, 0.0f}, "#000000"} {}

LevelC::LevelC(Vector2 origin, const char *bgColourHexCode)
    : Scene{origin, bgColourHexCode} {}

LevelC::~LevelC()
{
    shutdown();
}

void LevelC::initialise()
{
    mPlayerOnSpikes = false;
    mGameState.nextSceneID = NO_SCENE;

    // Level 3 audio and image assets; replace these paths to use different art.
    mGameState.bgm = LoadMusicStream("assets/BGM Music/Level 3 BGM.mp3");
    SetMusicVolume(mGameState.bgm, 0.5f);
    PlayMusicStream(mGameState.bgm);
    mGameState.jumpSound = LoadSound("assets/Misc. Sound/Jump Sound.mp3");

    mBackground = LoadTexture(
        "assets/Tiles and Background/Dark Background.png");
    mHealthBar = LoadTexture("assets/Health Bar.png");
    mAngelStatue = LoadTexture(
        "assets/Tiles and Background/Angel Statue.png");
    mDecorAtlas = LoadTexture("assets/Tiles and Background/Decor.png");

    buildLevelData();
    mGameState.map = new Map(
        LEVEL_C_MAP_COLUMNS,
        LEVEL_C_MAP_ROWS,
        mLevelData,
        "assets/Tiles and Background/Floor Tiles.png",
        LEVEL_C_TILE_SIZE,
        9,
        18,
        mOrigin);

    const std::vector<int> runningFrames = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    const std::map<Direction, std::vector<int>> animationAtlas = {
        {DOWN, runningFrames},
        {LEFT, runningFrames},
        {UP, runningFrames},
        {RIGHT, runningFrames}};

    mGameState.player = new Entity(
        {0.0f, 0.0f},
        {120.0f, 80.0f},
        "assets/Protag/_Run.png",
        ATLAS,
        {1, 10},
        animationAtlas,
        PLAYER);
    mGameState.player->setIdleAnimation(
        "assets/Protag/_Idle.png",
        {1, 10},
        runningFrames,
        8);
    mGameState.player->setTurnAroundAnimation(
        "assets/Protag/_TurnAround.png",
        {1, 3},
        {0, 1, 2});

    const float groundY = mGameState.map->getTopBoundary() +
                          18.0f * LEVEL_C_TILE_SIZE;
    mPlayerStartPosition = {
        mGameState.map->getLeftBoundary() + 4.0f * LEVEL_C_TILE_SIZE,
        groundY - mGameState.player->getScale().y / 2.0f};
    mGameState.player->setPosition(mPlayerStartPosition);
    mGameState.player->setJumpingPower(520.0f);
    mGameState.player->setColliderDimensions({30.0f, 40.0f});
    mGameState.player->setColliderOffset({-4.0f, 20.0f});
    mGameState.player->setAcceleration({0.0f, SCENE_GRAVITY});

    const float statueWidth = mAngelStatue.width * LEVEL_C_STATUE_SCALE;
    const float statueHeight = mAngelStatue.height * LEVEL_C_STATUE_SCALE;
    const float statueColumn = LEVEL_C_MAP_COLUMNS -
        statueWidth / (2.0f * LEVEL_C_TILE_SIZE);
    const float statueX =
        mGameState.map->getLeftBoundary() +
        statueColumn * LEVEL_C_TILE_SIZE;
    const float statueGroundY =
        getGroundYAtColumn(static_cast<int>(statueColumn));
    mLevelExit.setPosition({
        statueX,
        statueGroundY - statueHeight / 2.0f
    });
    mLevelExit.setColliderDimensions({statueWidth, statueHeight});
}

void LevelC::buildLevelData()
{
    for (int tileIndex = 0;
         tileIndex < LEVEL_C_MAP_COLUMNS * LEVEL_C_MAP_ROWS;
         tileIndex++)
    {
        mLevelData[tileIndex] = 0;
        mSpikeTiles[tileIndex] = false;
        mGroundSpikeTiles[tileIndex] = false;
    }

    // Main route: separate ground runs leave short gaps for jumps and falls.
    addCavePlatform(
        mLevelData, LEVEL_C_MAP_COLUMNS, LEVEL_C_MAP_ROWS, 0, 20, 18, 3);
    addCavePlatform(
        mLevelData, LEVEL_C_MAP_COLUMNS, LEVEL_C_MAP_ROWS, 23, 39, 18, 3);
    addCavePlatform(
        mLevelData, LEVEL_C_MAP_COLUMNS, LEVEL_C_MAP_ROWS, 42, 57, 18, 3);
    addCavePlatform(
        mLevelData, LEVEL_C_MAP_COLUMNS, LEVEL_C_MAP_ROWS, 60, 75, 18, 3);
    addCavePlatform(
        mLevelData, LEVEL_C_MAP_COLUMNS, LEVEL_C_MAP_ROWS, 78, 95, 18, 3);
    addCavePlatform(
        mLevelData, LEVEL_C_MAP_COLUMNS, LEVEL_C_MAP_ROWS, 98, 123, 18, 3);

    // Raised stepping platforms make the route harder than the earlier levels.
    addCavePlatform(
        mLevelData, LEVEL_C_MAP_COLUMNS, LEVEL_C_MAP_ROWS, 7, 11, 15, 0);
    addCavePlatform(
        mLevelData, LEVEL_C_MAP_COLUMNS, LEVEL_C_MAP_ROWS, 14, 18, 13, 0);
    addCavePlatform(
        mLevelData, LEVEL_C_MAP_COLUMNS, LEVEL_C_MAP_ROWS, 19, 24, 15, 0);
    addCavePlatform(
        mLevelData, LEVEL_C_MAP_COLUMNS, LEVEL_C_MAP_ROWS, 27, 32, 14, 0);
    addCavePlatform(
        mLevelData, LEVEL_C_MAP_COLUMNS, LEVEL_C_MAP_ROWS, 35, 40, 12, 0);
    addCavePlatform(
        mLevelData, LEVEL_C_MAP_COLUMNS, LEVEL_C_MAP_ROWS, 45, 50, 15, 0);
    addCavePlatform(
        mLevelData, LEVEL_C_MAP_COLUMNS, LEVEL_C_MAP_ROWS, 53, 58, 13, 0);
    addCavePlatform(
        mLevelData, LEVEL_C_MAP_COLUMNS, LEVEL_C_MAP_ROWS, 62, 67, 15, 0);
    addCavePlatform(
        mLevelData, LEVEL_C_MAP_COLUMNS, LEVEL_C_MAP_ROWS, 70, 76, 13, 0);
    addCavePlatform(
        mLevelData, LEVEL_C_MAP_COLUMNS, LEVEL_C_MAP_ROWS, 82, 87, 15, 0);
    addCavePlatform(
        mLevelData, LEVEL_C_MAP_COLUMNS, LEVEL_C_MAP_ROWS, 90, 96, 13, 0);
    addCavePlatform(
        mLevelData, LEVEL_C_MAP_COLUMNS, LEVEL_C_MAP_ROWS, 101, 106, 15, 0);
    addCavePlatform(
        mLevelData, LEVEL_C_MAP_COLUMNS, LEVEL_C_MAP_ROWS, 109, 115, 13, 0);
    addCavePlatform(
        mLevelData, LEVEL_C_MAP_COLUMNS, LEVEL_C_MAP_ROWS, 118, 121, 15, 0);

    // Roofs use the same tile helper, with fill rows extending downward.
    addCavePlatform(
        mLevelData, LEVEL_C_MAP_COLUMNS, LEVEL_C_MAP_ROWS, 21, 40, 7, 2);
    addCavePlatform(
        mLevelData, LEVEL_C_MAP_COLUMNS, LEVEL_C_MAP_ROWS, 47, 64, 10, 1);
    addCavePlatform(
        mLevelData, LEVEL_C_MAP_COLUMNS, LEVEL_C_MAP_ROWS, 72, 91, 7, 2);
    addCavePlatform(
        mLevelData, LEVEL_C_MAP_COLUMNS, LEVEL_C_MAP_ROWS, 99, 121, 9, 1);

    // Ground spikes face up; overhead hazards keep their original orientation.
    addSpikeRun(
        mLevelData, mSpikeTiles, mGroundSpikeTiles,
        LEVEL_C_MAP_COLUMNS, 18, 12, 13, true);
    addSpikeRun(
        mLevelData, mSpikeTiles, mGroundSpikeTiles,
        LEVEL_C_MAP_COLUMNS, 18, 30, 31, true);
    addSpikeRun(
        mLevelData, mSpikeTiles, mGroundSpikeTiles,
        LEVEL_C_MAP_COLUMNS, 18, 51, 52, true);
    addSpikeRun(
        mLevelData, mSpikeTiles, mGroundSpikeTiles,
        LEVEL_C_MAP_COLUMNS, 18, 68, 69, true);
    addSpikeRun(
        mLevelData, mSpikeTiles, mGroundSpikeTiles,
        LEVEL_C_MAP_COLUMNS, 18, 88, 90, true);
    addSpikeRun(
        mLevelData, mSpikeTiles, mGroundSpikeTiles,
        LEVEL_C_MAP_COLUMNS, 18, 110, 111, true);
    addSpikeRun(
        mLevelData, mSpikeTiles, mGroundSpikeTiles,
        LEVEL_C_MAP_COLUMNS, 10, 31, 33, false);
    addSpikeRun(
        mLevelData, mSpikeTiles, mGroundSpikeTiles,
        LEVEL_C_MAP_COLUMNS, 12, 53, 54, false);
    addSpikeRun(
        mLevelData, mSpikeTiles, mGroundSpikeTiles,
        LEVEL_C_MAP_COLUMNS, 10, 80, 82, false);
    addSpikeRun(
        mLevelData, mSpikeTiles, mGroundSpikeTiles,
        LEVEL_C_MAP_COLUMNS, 11, 108, 110, false);
}

void LevelC::update(float deltaTime)
{
    UpdateMusicStream(mGameState.bgm);
    mGameState.player->update(
        deltaTime,
        nullptr,
        mGameState.map,
        nullptr,
        0);

    if (mGameState.player->getPosition().y > SCENE_FALL_THRESHOLD)
    {
        loseLife();
        if (mGameState.nextSceneID == LOSE_SCREEN)
            return;

        mGameState.player->setPosition(mPlayerStartPosition);
        mGameState.player->setVelocity({0.0f, 0.0f});
        mGameState.player->setMovement({0.0f, 0.0f});
        mPlayerOnSpikes = false;
    }

    if (mGameState.player->collidesWith(&mLevelExit))
    {
        mGameState.nextSceneID = WIN_SCREEN;
        return;
    }

    // Only charge once until the player leaves the current spike contact.
    const bool touchingSpikes = isPlayerTouchingSpikes();
    if (touchingSpikes && !mPlayerOnSpikes)
    {
        loseLife();
        if (mGameState.nextSceneID == LOSE_SCREEN)
            return;
    }
    mPlayerOnSpikes = touchingSpikes;
}

bool LevelC::isPlayerTouchingSpikes() const
{
    // Check the player's actual collider against the separate spike flags.
    const Vector2 colliderPosition =
        mGameState.player->getColliderPosition();
    const Vector2 colliderDimensions =
        mGameState.player->getColliderDimensions();
    const float colliderLeft =
        colliderPosition.x - colliderDimensions.x / 2.0f - 0.1f;
    const float colliderRight =
        colliderPosition.x + colliderDimensions.x / 2.0f + 0.1f;
    const float colliderTop =
        colliderPosition.y - colliderDimensions.y / 2.0f - 0.1f;
    const float colliderBottom =
        colliderPosition.y + colliderDimensions.y / 2.0f + 0.1f;
    const float mapLeft = mGameState.map->getLeftBoundary();
    const float mapTop = mGameState.map->getTopBoundary();
    const float tileSize = mGameState.map->getTileSize();

    if (colliderRight <= mapLeft ||
        colliderLeft >= mGameState.map->getRightBoundary() ||
        colliderBottom <= mapTop ||
        colliderTop >= mGameState.map->getBottomBoundary())
        return false;

    int firstColumn = static_cast<int>((colliderLeft - mapLeft) / tileSize);
    int lastColumn = static_cast<int>((colliderRight - mapLeft) / tileSize);
    int firstRow = static_cast<int>((colliderTop - mapTop) / tileSize);
    int lastRow = static_cast<int>((colliderBottom - mapTop) / tileSize);

    if (firstColumn < 0)
        firstColumn = 0;
    if (lastColumn >= LEVEL_C_MAP_COLUMNS)
        lastColumn = LEVEL_C_MAP_COLUMNS - 1;
    if (firstRow < 0)
        firstRow = 0;
    if (lastRow >= LEVEL_C_MAP_ROWS)
        lastRow = LEVEL_C_MAP_ROWS - 1;

    for (int row = firstRow; row <= lastRow; row++)
    {
        const float tileTop = mapTop + row * tileSize;
        const float tileBottom = tileTop + tileSize;
        if (colliderTop >= tileBottom || colliderBottom < tileTop)
            continue;

        for (int column = firstColumn; column <= lastColumn; column++)
        {
            const float tileLeft = mapLeft + column * tileSize;
            const float tileRight = tileLeft + tileSize;
            if (colliderLeft >= tileRight || colliderRight <= tileLeft)
                continue;

            const int tileIndex = row * LEVEL_C_MAP_COLUMNS + column;
            if (mSpikeTiles[tileIndex])
                return true;
        }
    }

    return false;
}

void LevelC::loseLife()
{
    // Falls and spikes share the game's persistent lives and hurt sound.
    if (gGameProgress.lives > 0)
    {
        gGameProgress.lives--;
        PlaySound(gGameProgress.hurtSound);
    }

    if (gGameProgress.lives <= 0)
        mGameState.nextSceneID = LOSE_SCREEN;
}

void LevelC::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));

    const Vector2 playerPosition = mGameState.player->getPosition();
    const float backgroundWidth = 2.0f * mOrigin.x;
    const float backgroundHeight = 2.0f * mOrigin.y;
    const float backgroundOffset = playerPosition.x * 0.2f;
    const float backgroundLeft =
        playerPosition.x - mOrigin.x - backgroundOffset;

    drawTexture(mBackground, {backgroundLeft - backgroundWidth, 0.0f,
                              backgroundWidth, backgroundHeight});
    drawTexture(mBackground, {backgroundLeft, 0.0f,
                              backgroundWidth, backgroundHeight});
    drawTexture(mBackground, {backgroundLeft + backgroundWidth, 0.0f,
                              backgroundWidth, backgroundHeight});

    mGameState.map->render();
    renderSpikes();

    // Each decoration sits on the first solid platform or floor tile below it.
    drawDecorationAtGround(
        mDecorAtlas, {64.0f, 0.0f, 32.0f, 32.0f},
        8.5f, 1.6f);
    drawDecorationAtGround(
        mDecorAtlas, {96.0f, 0.0f, 32.0f, 32.0f},
        29.5f, 1.6f);
    drawDecorationAtGround(
        mDecorAtlas, {64.0f, 32.0f, 32.0f, 32.0f},
        48.5f, 1.7f);
    drawDecorationAtGround(
        mDecorAtlas, {96.0f, 0.0f, 32.0f, 32.0f},
        73.5f, 1.5f);
    drawDecorationAtGround(
        mDecorAtlas, {64.0f, 32.0f, 32.0f, 32.0f},
        103.5f, 1.7f);

    // Large stone piles are drawn from separate atlas cuts and spread out.
    drawDecorationAtGround(
        mDecorAtlas, {0.0f, 288.0f, 64.0f, 64.0f},
        16.5f, 1.1f);
    drawDecorationAtGround(
        mDecorAtlas, {64.0f, 288.0f, 64.0f, 64.0f},
        43.5f, 1.1f);
    drawDecorationAtGround(
        mDecorAtlas, {128.0f, 288.0f, 64.0f, 64.0f},
        66.5f, 1.1f);
    drawDecorationAtGround(
        mDecorAtlas, {0.0f, 288.0f, 64.0f, 64.0f},
        92.5f, 1.1f);
    drawDecorationAtGround(
        mDecorAtlas, {64.0f, 288.0f, 64.0f, 64.0f},
        114.5f, 1.1f);

    // Scarecrow atlas crop, placed in two different sections of the level.
    drawDecorationAtGround(
        mDecorAtlas, {288.0f, 192.0f, 64.0f, 96.0f},
        37.5f, 1.0f);
    drawDecorationAtGround(
        mDecorAtlas, {288.0f, 192.0f, 64.0f, 96.0f},
        84.5f, 1.0f);

    // Small debris is near the middle; the statue marks the far end.
    drawDecorationAtGround(
        mDecorAtlas, {32.0f, 224.0f, 64.0f, 32.0f},
        57.5f, 1.0f);

    const Vector2 statuePosition = mLevelExit.getPosition();
    const float statueGroundY =
                               statuePosition.y + mAngelStatue.height * LEVEL_C_STATUE_SCALE / 2.0f;
    drawAtlasItemAtGround(
        mAngelStatue,
        {0.0f, 0.0f, static_cast<float>(mAngelStatue.width),
         static_cast<float>(mAngelStatue.height)},
                               statuePosition.x,
                               statueGroundY,
                               LEVEL_C_STATUE_SCALE);

    mGameState.player->render();
    renderHealthBar();
}

float LevelC::getGroundYAtColumn(int column) const
{
    const float mapTop = mGameState.map->getTopBoundary();

    // Rows 12-18 contain the raised platforms and main floor; roofs are above.
    for (int row = 12; row <= 18; row++)
    {
        if (mLevelData[row * LEVEL_C_MAP_COLUMNS + column] != 0)
            return mapTop + row * LEVEL_C_TILE_SIZE;
    }

    return mGameState.map->getBottomBoundary();
}

void LevelC::drawDecorationAtGround(
    Texture2D texture,
    Rectangle source,
    float tileColumn,
    float scale)
{
    const float centerX =
        mGameState.map->getLeftBoundary() + tileColumn * LEVEL_C_TILE_SIZE;
    const int column = static_cast<int>(tileColumn);
    const float groundY = getGroundYAtColumn(column);

    drawAtlasItemAtGround(texture, source, centerX, groundY, scale);
}

void LevelC::renderSpikes()
{
    const Texture2D texture = mGameState.map->getTextureAtlas();
    const float tileSize = mGameState.map->getTileSize();
    const float mapLeft = mGameState.map->getLeftBoundary();
    const float mapTop = mGameState.map->getTopBoundary();

    for (int row = 0; row < LEVEL_C_MAP_ROWS; row++)
    {
        for (int column = 0; column < LEVEL_C_MAP_COLUMNS; column++)
        {
            const int tileIndex = row * LEVEL_C_MAP_COLUMNS + column;
            if (!mSpikeTiles[tileIndex])
                continue;

            // Alternate the two spike atlas tiles used by the original layout.
            const int spikeTileID = column % 2 == 0 ? 89 : 90;
            const Rectangle source = getUVRectangle(
                &texture,
                spikeTileID - 1,
                mGameState.map->getTextureRows(),
                mGameState.map->getTextureColumns());
            const float rotation =
                mGroundSpikeTiles[tileIndex] ? 180.0f : 0.0f;
            const float x = mapLeft + column * tileSize;
            const float y = mapTop + row * tileSize;

            DrawTexturePro(
                texture,
                source,
                {x, y, tileSize, tileSize},
                {tileSize / 2.0f, tileSize / 2.0f},
                rotation,
                WHITE);
        }
    }
}

void LevelC::renderHealthBar()
{
    const float screenLeft =
        mGameState.player->getPosition().x - mOrigin.x + 14.0f;
    const float fillWidth = 82.0f * (gGameProgress.lives / 5.0f);

    DrawTexturePro(
        mHealthBar,
        {42.0f, 42.0f, fillWidth, 14.0f},
        {screenLeft + 42.0f, 25.0f, fillWidth, 14.0f},
        {0.0f, 0.0f},
        0.0f,
        WHITE);
    DrawTexturePro(
        mHealthBar,
        {0.0f, 0.0f, 128.0f, 36.0f},
        {screenLeft, 12.0f, 128.0f, 36.0f},
        {0.0f, 0.0f},
        0.0f,
        WHITE);
}

void LevelC::shutdown()
{
    delete mGameState.player;
    mGameState.player = nullptr;
    delete mGameState.map;
    mGameState.map = nullptr;

    UnloadTexture(mBackground);
    mBackground = {};
    UnloadTexture(mHealthBar);
    mHealthBar = {};
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
