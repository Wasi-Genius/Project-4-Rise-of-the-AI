#include "LevelA.h"

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
        unsigned int topTile = 2;
        if (column == startColumn) topTile = 1;
        else if (column == endColumn) topTile = 3;

        levelData[topRow * mapColumns + column] = topTile;

        for (int depth = 1; depth <= fillRows && topRow + depth < mapRows; depth++)
            levelData[(topRow + depth) * mapColumns + column] = 11;
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

void drawHuntressFrame(
    Texture2D texture,
    int frame,
    Vector2 position,
    Vector2 scale,
    Direction direction
)
{
    const int frameCount = texture.width / texture.height;
    Rectangle source = getUVRectangle(&texture, frame, 1, frameCount);
    if (direction == LEFT) source.width *= -1.0f;

    DrawTexturePro(
        texture,
        source,
        { position.x, position.y, scale.x, scale.y },
        { scale.x / 2.0f, scale.y / 2.0f },
        0.0f,
        WHITE
    );
}
}

// The default and configured constructors pass this scene's coordinate
// origin and background colour to the shared Scene base.
LevelA::LevelA() : Scene{ { 0.0f, 0.0f }, "#000000" } {}

LevelA::LevelA(Vector2 origin, const char *bgColourHexCode)
    : Scene{ origin, bgColourHexCode } {}

LevelA::~LevelA()
{
    shutdown();
}

void LevelA::initialise()
{
    mGameState.bgm = LoadMusicStream("assets/BGM Music/Level 1 BGM.mp3");
    SetMusicVolume(mGameState.bgm, 0.5f);
    PlayMusicStream(mGameState.bgm);
    mGameState.jumpSound = LoadSound("assets/Misc. Sound/Jump Sound.mp3");
    mGameState.nextSceneID = NO_SCENE;

    mBackground = LoadTexture(
        "assets/Tiles and Background/Blue Background.png"
    );
    mBlueBirch = LoadTexture("assets/Tiles and Background/Blue Birch.png");
    mGreenBirch = LoadTexture("assets/Tiles and Background/Green Birch.png");
    mRedBirch = LoadTexture("assets/Tiles and Background/Red Birch.png");
    mSmallTent = LoadTexture("assets/Tiles and Background/Small Tent.png");
    mAngelStatue = LoadTexture("assets/Tiles and Background/Angel Statue.png");
    mHuntressIdle = LoadTexture("assets/Huntress/Idle.png");
    mHuntressRun = LoadTexture("assets/Huntress/Run.png");
    mHuntressJump = LoadTexture("assets/Huntress/Jump.png");
    mHuntressFall = LoadTexture("assets/Huntress/Fall.png");
    mHuntressAttackOne = LoadTexture("assets/Huntress/Attack1.png");
    mHuntressAttackTwo = LoadTexture("assets/Huntress/Attack2.png");

    buildLevelData();
    mGameState.map = new Map(
        LEVEL_A_MAP_COLUMNS,
        LEVEL_A_MAP_ROWS,
        mLevelData,
        "assets/Tiles and Background/Floor Tiles.png",
        LEVEL_A_TILE_SIZE,
        9,
        18,
        mOrigin
    );

    std::vector<int> runningFrames = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9
    };
    std::map<Direction, std::vector<int>> animationAtlas = {
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
        15.0f * LEVEL_A_TILE_SIZE;
    mPlayerStartPosition = {
        mGameState.map->getLeftBoundary() + 4.0f * LEVEL_A_TILE_SIZE,
        groundY - mGameState.player->getScale().y / 2.0f
    };
    mGameState.player->setPosition(mPlayerStartPosition);
    mGameState.player->setJumpingPower(520.0f);
    mGameState.player->setColliderDimensions({ 30.0f, 40.0f });
    mGameState.player->setColliderOffset({ -4.0f, 20.0f });
    mGameState.player->setAcceleration({ 0.0f, SCENE_GRAVITY });

    mHuntress = new Entity();
    mHuntress->setScale({ 150.0f, 150.0f });
    mHuntress->setColliderDimensions({ 44.0f, 58.0f });
    mHuntressStartPosition = {
        mGameState.map->getLeftBoundary() +
            LEVEL_A_MAP_COLUMNS * 0.25f * LEVEL_A_TILE_SIZE,
        groundY - mHuntress->getColliderDimensions().y / 2.0f
    };
    mHuntress->setPosition(mHuntressStartPosition);
    mHuntress->setEntityType(NPC);
    mHuntress->setAIType(FOLLOWER);
    mHuntress->setAIState(IDLE);
    mHuntress->setDirection(LEFT);
    mHuntress->setSpeed(220);
    mHuntress->setJumpingPower(520.0f);
    mHuntress->setAcceleration({ 0.0f, SCENE_GRAVITY });
}

void LevelA::buildLevelData()
{
    addPlatform(
        mLevelData, LEVEL_A_MAP_COLUMNS, LEVEL_A_MAP_ROWS, 0, 23, 15, 2
    );
    addPlatform(
        mLevelData, LEVEL_A_MAP_COLUMNS, LEVEL_A_MAP_ROWS, 27, 83, 15, 2
    );

    addPlatform(
        mLevelData, LEVEL_A_MAP_COLUMNS, LEVEL_A_MAP_ROWS, 8, 12, 13, 0
    );
    addPlatform(
        mLevelData, LEVEL_A_MAP_COLUMNS, LEVEL_A_MAP_ROWS, 15, 19, 11, 0
    );
    addPlatform(
        mLevelData, LEVEL_A_MAP_COLUMNS, LEVEL_A_MAP_ROWS, 22, 26, 12, 0
    );
    addPlatform(
        mLevelData, LEVEL_A_MAP_COLUMNS, LEVEL_A_MAP_ROWS, 36, 40, 13, 0
    );
    addPlatform(
        mLevelData, LEVEL_A_MAP_COLUMNS, LEVEL_A_MAP_ROWS, 49, 53, 11, 0
    );
    addPlatform(
        mLevelData, LEVEL_A_MAP_COLUMNS, LEVEL_A_MAP_ROWS, 62, 66, 13, 0
    );
    addPlatform(
        mLevelData, LEVEL_A_MAP_COLUMNS, LEVEL_A_MAP_ROWS, 75, 79, 11, 0
    );
}

void LevelA::update(float deltaTime)
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
        mGameState.player->setPosition(mPlayerStartPosition);
        mGameState.player->setVelocity({ 0.0f, 0.0f });
        mGameState.player->setMovement({ 0.0f, 0.0f });
    }

    updateHuntress(deltaTime);
}

void LevelA::updateHuntress(float deltaTime)
{
    const bool isAttacking =
        mHuntressAnimation == HUNTRESS_ATTACK_ONE ||
        mHuntressAnimation == HUNTRESS_ATTACK_TWO;

    if (isAttacking)
        mHuntress->setMovement({ 0.0f, 0.0f });
    else if (mHuntress->getAIState() == FOLLOWING &&
             mHuntress->isCollidingBottom())
    {
        const float direction =
            mHuntress->getDirection() == LEFT ? -1.0f : 1.0f;
        const Vector2 position = mHuntress->getPosition();
        const Vector2 collider = mHuntress->getColliderDimensions();
        const Vector2 groundAhead = {
            position.x + direction *
                (collider.x / 2.0f + LEVEL_A_TILE_SIZE / 2.0f),
            position.y + collider.y / 2.0f + 2.0f
        };
        float xOverlap = 0.0f;
        float yOverlap = 0.0f;
        const bool hasGroundAhead = mGameState.map->isSolidTileAt(
            groundAhead,
            &xOverlap,
            &yOverlap
        );
        const bool playerIsAbove =
            mGameState.player->getPosition().y < position.y - 40.0f;

        if (playerIsAbove || !hasGroundAhead)
        {
            mHuntress->jump();
            PlaySound(mGameState.jumpSound);
        }
    }

    mHuntress->update(
        deltaTime,
        isAttacking ? nullptr : mGameState.player,
        mGameState.map,
        nullptr,
        0
    );

    if (mHuntress->getPosition().y > SCENE_FALL_THRESHOLD)
    {
        mHuntress->setPosition(mHuntressStartPosition);
        mHuntress->setVelocity({ 0.0f, 0.0f });
        mHuntress->setMovement({ 0.0f, 0.0f });
        setHuntressAnimation(HUNTRESS_IDLE);
    }

    const bool isFollowing =
        mHuntress->getAIState() == FOLLOWING ||
        mHuntress->getAIState() == WALKING;
    const bool isNearPlayer = isFollowing &&
        Vector2Distance(
            mHuntress->getPosition(),
            mGameState.player->getPosition()
        ) < 130.0f;

    if (isAttacking)
    {
        if (!mHuntressAnimationFinished)
        {
            animateHuntress(deltaTime);
            return;
        }

        if (isNearPlayer)
        {
            setHuntressAnimation(
                mHuntressAttackTwoNext
                    ? HUNTRESS_ATTACK_TWO
                    : HUNTRESS_ATTACK_ONE
            );
            mHuntressAttackTwoNext = !mHuntressAttackTwoNext;
        }
        else
            setHuntressAnimation(
                getHuntressMovementAnimation(isFollowing)
            );
        return;
    }

    if (isNearPlayer)
    {
        setHuntressAnimation(
            mHuntressAttackTwoNext ? HUNTRESS_ATTACK_TWO : HUNTRESS_ATTACK_ONE
        );
        mHuntressAttackTwoNext = !mHuntressAttackTwoNext;
    }
    else
        setHuntressAnimation(getHuntressMovementAnimation(isFollowing));

    animateHuntress(deltaTime);
}

void LevelA::setHuntressAnimation(HuntressAnimation animation)
{
    if (mHuntressAnimation == animation) return;

    mHuntressAnimation = animation;
    mHuntressAnimationFrame = 0;
    mHuntressAnimationTime = 0.0f;
    mHuntressAnimationFinished = false;
}

LevelA::HuntressAnimation LevelA::getHuntressMovementAnimation(
    bool isFollowing
) const
{
    if (!isFollowing) return HUNTRESS_IDLE;

    const float verticalVelocity = mHuntress->getVelocity().y;
    if (verticalVelocity < -1.0f) return HUNTRESS_JUMP;
    if (verticalVelocity > 1.0f) return HUNTRESS_FALL;
    if (GetLength(mHuntress->getMovement()) > 0.0f) return HUNTRESS_RUN;

    return HUNTRESS_IDLE;
}

Texture2D LevelA::getHuntressAnimationTexture() const
{
    switch (mHuntressAnimation)
    {
    case HUNTRESS_RUN: return mHuntressRun;
    case HUNTRESS_JUMP: return mHuntressJump;
    case HUNTRESS_FALL: return mHuntressFall;
    case HUNTRESS_ATTACK_ONE: return mHuntressAttackOne;
    case HUNTRESS_ATTACK_TWO: return mHuntressAttackTwo;
    case HUNTRESS_IDLE:
    default: return mHuntressIdle;
    }
}

void LevelA::animateHuntress(float deltaTime)
{
    const Texture2D texture = getHuntressAnimationTexture();
    const int frameCount = texture.width / texture.height;
    const float secondsPerFrame =
        1.0f / static_cast<float>(Entity::DEFAULT_FRAME_SPEED);

    mHuntressAnimationTime += deltaTime;
    while (mHuntressAnimationTime >= secondsPerFrame)
    {
        mHuntressAnimationTime -= secondsPerFrame;
        if (mHuntressAnimationFrame + 1 < frameCount)
            mHuntressAnimationFrame++;
        else if (mHuntressAnimation == HUNTRESS_ATTACK_ONE ||
                 mHuntressAnimation == HUNTRESS_ATTACK_TWO)
        {
            mHuntressAnimationFinished = true;
            break;
        }
        else
            mHuntressAnimationFrame = 0;
    }
}

void LevelA::render()
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
        15.0f * LEVEL_A_TILE_SIZE;
    drawAtGround(mBlueBirch, mapLeft + 5.0f * LEVEL_A_TILE_SIZE, groundY, 1.5f);
    drawAtGround(mGreenBirch, mapLeft + 13.0f * LEVEL_A_TILE_SIZE, groundY, 1.5f);
    drawAtGround(mSmallTent, mapLeft + 20.0f * LEVEL_A_TILE_SIZE, groundY, 1.5f);
    drawAtGround(mAngelStatue, mapLeft + 34.0f * LEVEL_A_TILE_SIZE, groundY, 1.5f);
    drawAtGround(mRedBirch, mapLeft + 45.0f * LEVEL_A_TILE_SIZE, groundY, 1.5f);
    drawAtGround(mSmallTent, mapLeft + 58.0f * LEVEL_A_TILE_SIZE, groundY, 1.5f);
    drawAtGround(mBlueBirch, mapLeft + 70.0f * LEVEL_A_TILE_SIZE, groundY, 1.5f);

    renderHuntress();
    mGameState.player->render();
}

void LevelA::renderHuntress()
{
    const Texture2D texture = getHuntressAnimationTexture();
    // The sprite sheets leave transparent space below the Huntress.
    const float spriteFootY = 96.0f;
    Vector2 renderPosition = mHuntress->getPosition();
    renderPosition.y += mHuntress->getColliderDimensions().y / 2.0f +
        mHuntress->getScale().y / 2.0f -
        spriteFootY * mHuntress->getScale().y / texture.height;

    drawHuntressFrame(
        texture,
        mHuntressAnimationFrame,
        renderPosition,
        mHuntress->getScale(),
        mHuntress->getDirection()
    );
}

void LevelA::shutdown()
{
    // Clearing each resource after releasing it makes this safe to call when
    // switching scenes and again from the destructor.
    delete mHuntress;
    mHuntress = nullptr;
    delete mGameState.player;
    mGameState.player = nullptr;
    delete mGameState.map;
    mGameState.map = nullptr;

    UnloadTexture(mBackground);
    mBackground = {};
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
    UnloadTexture(mHuntressIdle);
    mHuntressIdle = {};
    UnloadTexture(mHuntressRun);
    mHuntressRun = {};
    UnloadTexture(mHuntressJump);
    mHuntressJump = {};
    UnloadTexture(mHuntressFall);
    mHuntressFall = {};
    UnloadTexture(mHuntressAttackOne);
    mHuntressAttackOne = {};
    UnloadTexture(mHuntressAttackTwo);
    mHuntressAttackTwo = {};

    UnloadMusicStream(mGameState.bgm);
    mGameState.bgm = {};
    UnloadSound(mGameState.jumpSound);
    mGameState.jumpSound = {};
    mGameState.nextSceneID = NO_SCENE;
}
