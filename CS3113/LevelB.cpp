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

void drawWarriorFrame(
    Texture2D texture,
    int frame,
    Vector2 position,
    Direction direction
)
{
    const int frameCount = texture.width / texture.height;
    Rectangle source = getUVRectangle(&texture, frame, 1, frameCount);
    if (direction == LEFT) source.width *= -1.0f;

    DrawTexturePro(
        texture,
        source,
        { position.x - 81.0f, position.y - 81.0f, 162.0f, 162.0f },
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
    mWarriorAnimation = WARRIOR_IDLE;
    mWarriorAnimationFrame = 0;
    mWarriorAnimationTime = 0.0f;
    mWarriorJumpTime = 0.0f;
    mWarriorAnimationFinished = false;
    mWarriorDamageApplied = false;
    mWarriorAttackTwoNext = false;

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
    mWarriorIdle = LoadTexture("assets/Warrior/Idle.png");
    mWarriorRun = LoadTexture("assets/Warrior/Run.png");
    mWarriorJump = LoadTexture("assets/Warrior/Jump.png");
    mWarriorFall = LoadTexture("assets/Warrior/Fall.png");
    mWarriorAttackOne = LoadTexture("assets/Warrior/Attack1.png");
    mWarriorAttackTwo = LoadTexture("assets/Warrior/Attack2.png");

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
    mGameState.player->setIdleAnimation(
        "assets/Protag/_Idle.png",
        { 1, 10 },
        runningFrames,
        8
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

    mWarrior = new Entity();
    mWarrior->setScale({ 162.0f, 162.0f });
    mWarrior->setColliderDimensions({ 34.0f, 42.0f });
    const float warriorGroundY = mGameState.map->getTopBoundary() +
        17.0f * LEVEL_B_TILE_SIZE;
    mWarriorStartPosition = {
        mGameState.map->getLeftBoundary() + 28.0f * LEVEL_B_TILE_SIZE,
        warriorGroundY - 21.0f
    };
    mWarrior->setPosition(mWarriorStartPosition);
    mWarrior->setEntityType(NPC);
    mWarrior->setAIType(WANDERER);
    mWarrior->moveRight();
    mWarrior->setSpeed(150);
    mWarrior->setJumpingPower(520.0f);
    mWarrior->setAcceleration({ 0.0f, SCENE_GRAVITY });
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

    updateWarrior(deltaTime);
}

void LevelB::updateWarrior(float deltaTime)
{
    catchUpWarriorIfOffScreen();

    const bool isAttackAnimation =
        mWarriorAnimation == WARRIOR_ATTACK_ONE ||
        mWarriorAnimation == WARRIOR_ATTACK_TWO;
    bool isNearPlayer = mGameState.player->collidesWith(mWarrior);

    if (isAttackAnimation && !isNearPlayer)
    {
        mWarrior->setEntityType(NPC);
        mWarrior->setMovement({ 0.0f, 0.0f });
        setWarriorAnimation(getWarriorMovementAnimation());
    }

    const bool isAttacking =
        mWarriorAnimation == WARRIOR_ATTACK_ONE ||
        mWarriorAnimation == WARRIOR_ATTACK_TWO;

    if (!isAttacking && isNearPlayer)
    {
        if (mGameState.player->getPosition().x < mWarrior->getPosition().x)
            mWarrior->moveLeft();
        else
            mWarrior->moveRight();
        setWarriorAnimation(
            mWarriorAttackTwoNext
                ? WARRIOR_ATTACK_TWO
                : WARRIOR_ATTACK_ONE
        );
        mWarriorAttackTwoNext = !mWarriorAttackTwoNext;
        mWarrior->setEntityType(NONE);
        mWarrior->setMovement({ 0.0f, 0.0f });
    }
    else if (!isAttacking)
    {
        mWarrior->setEntityType(NPC);
        mWarriorJumpTime += deltaTime;

        if (mWarrior->isCollidingBottom())
        {
            const float direction =
                mWarrior->getDirection() == LEFT ? -1.0f : 1.0f;
            const Vector2 position = mWarrior->getPosition();
            const Vector2 collider = mWarrior->getColliderDimensions();
            const Vector2 groundAhead = {
                position.x + direction *
                    (collider.x / 2.0f + LEVEL_B_TILE_SIZE / 2.0f),
                position.y + collider.y / 2.0f + 2.0f
            };
            float xOverlap = 0.0f;
            float yOverlap = 0.0f;
            const bool hasGroundAhead = mGameState.map->isSolidTileAt(
                groundAhead,
                &xOverlap,
                &yOverlap
            );
            const bool pastMapEdge =
                groundAhead.x < mGameState.map->getLeftBoundary() ||
                groundAhead.x >= mGameState.map->getRightBoundary();

            const bool hasJumpClearance = hasWarriorJumpClearance();
            const bool shouldJump = (!hasGroundAhead && !pastMapEdge) ||
                mWarriorJumpTime >= 2.5f;

            if (shouldJump && hasJumpClearance && !pastMapEdge)
            {
                mWarrior->jump();
                mWarriorJumpTime = 0.0f;
                PlaySound(mGameState.jumpSound);
            }
            else if (!hasGroundAhead || pastMapEdge)
            {
                if (direction < 0.0f)
                    mWarrior->moveRight();
                else
                    mWarrior->moveLeft();
            }
            else if (mWarriorJumpTime >= 2.5f)
                mWarriorJumpTime = 0.0f;
        }
    }

    mWarrior->update(
        deltaTime,
        nullptr,
        mGameState.map,
        nullptr,
        0
    );

    if (mWarrior->getPosition().y > SCENE_FALL_THRESHOLD)
    {
        mWarrior->setPosition(mWarriorStartPosition);
        mWarrior->setVelocity({ 0.0f, 0.0f });
        mWarrior->setMovement({ 0.0f, 0.0f });
        mWarrior->setEntityType(NPC);
        mWarrior->moveRight();
        setWarriorAnimation(WARRIOR_IDLE);
        return;
    }

    isNearPlayer = mGameState.player->collidesWith(mWarrior);
    const bool isAttackActive =
        mWarriorAnimation == WARRIOR_ATTACK_ONE ||
        mWarriorAnimation == WARRIOR_ATTACK_TWO;

    if (isAttackActive && isNearPlayer && !mWarriorDamageApplied)
    {
        mWarriorDamageApplied = true;
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
    }

    if (isAttackActive && !isNearPlayer)
    {
        mWarrior->setEntityType(NPC);
        setWarriorAnimation(getWarriorMovementAnimation());
    }

    if (mWarriorAnimation == WARRIOR_ATTACK_ONE ||
        mWarriorAnimation == WARRIOR_ATTACK_TWO)
    {
        if (mWarriorAnimationFinished)
        {
            if (isNearPlayer)
            {
                if (mGameState.player->getPosition().x <
                    mWarrior->getPosition().x)
                    mWarrior->moveLeft();
                else
                    mWarrior->moveRight();
                mWarrior->setMovement({ 0.0f, 0.0f });
                setWarriorAnimation(
                    mWarriorAttackTwoNext
                        ? WARRIOR_ATTACK_TWO
                        : WARRIOR_ATTACK_ONE
                );
                mWarriorAttackTwoNext = !mWarriorAttackTwoNext;
            }
            else
            {
                mWarrior->setEntityType(NPC);
                setWarriorAnimation(getWarriorMovementAnimation());
            }
        }
        else
            animateWarrior(deltaTime);
    }
    else
    {
        if (isNearPlayer)
        {
            if (mGameState.player->getPosition().x <
                mWarrior->getPosition().x)
                mWarrior->moveLeft();
            else
                mWarrior->moveRight();
            mWarrior->setMovement({ 0.0f, 0.0f });
            setWarriorAnimation(
                mWarriorAttackTwoNext
                    ? WARRIOR_ATTACK_TWO
                    : WARRIOR_ATTACK_ONE
            );
            mWarriorAttackTwoNext = !mWarriorAttackTwoNext;
            mWarrior->setEntityType(NONE);
            mWarrior->setMovement({ 0.0f, 0.0f });
        }
        else
            setWarriorAnimation(getWarriorMovementAnimation());

        animateWarrior(deltaTime);
    }
}

void LevelB::catchUpWarriorIfOffScreen()
{
    const Vector2 playerPosition = mGameState.player->getPosition();
    const Vector2 warriorPosition = mWarrior->getPosition();
    const Vector2 warriorCollider = mWarrior->getColliderDimensions();
    const float screenLeft = playerPosition.x - mOrigin.x;
    const float warriorRight =
        warriorPosition.x + mWarrior->getScale().x / 2.0f;

    if (warriorRight >= screenLeft)
        return;

    const float minimumAhead =
        playerPosition.x + 2.0f * LEVEL_B_TILE_SIZE;
    const float preferredPosition =
        playerPosition.x + 4.0f * LEVEL_B_TILE_SIZE;
    const float maximumPosition =
        mGameState.map->getRightBoundary() -
        warriorCollider.x / 2.0f - 1.0f;
    const float groundY = mGameState.map->getTopBoundary() +
        17.0f * LEVEL_B_TILE_SIZE;
    const float warriorY = groundY - warriorCollider.y / 2.0f;
    const float sampleSpacing = LEVEL_B_TILE_SIZE / 2.0f;
    float catchUpPosition = 0.0f;
    bool foundCatchUpPosition = false;
    float xOverlap = 0.0f;
    float yOverlap = 0.0f;
    const auto hasSafeGround = [&](float candidateX)
    {
        for (float horizontalOffset = -warriorCollider.x / 2.0f;
             horizontalOffset <= warriorCollider.x / 2.0f;
             horizontalOffset += warriorCollider.x / 2.0f)
        {
            if (!mGameState.map->isSolidTileAt(
                    { candidateX + horizontalOffset, groundY + 1.0f },
                    &xOverlap,
                    &yOverlap
                ))
                return false;

            for (float sampleY = warriorY - warriorCollider.y / 2.0f + 1.0f;
                 sampleY < groundY;
                 sampleY += sampleSpacing)
            {
                if (mGameState.map->isSolidTileAt(
                        { candidateX + horizontalOffset, sampleY },
                        &xOverlap,
                        &yOverlap
                    ))
                    return false;
            }
        }

        return true;
    };

    for (float candidateX = preferredPosition;
         candidateX <= maximumPosition;
         candidateX += sampleSpacing)
    {
        if (hasSafeGround(candidateX))
        {
            catchUpPosition = candidateX;
            foundCatchUpPosition = true;
            break;
        }
    }

    if (!foundCatchUpPosition)
    {
        const float fallbackStart =
            preferredPosition - sampleSpacing < maximumPosition
                ? preferredPosition - sampleSpacing
                : maximumPosition;
        for (float candidateX = fallbackStart;
             candidateX >= minimumAhead;
             candidateX -= sampleSpacing)
        {
            if (hasSafeGround(candidateX))
            {
                catchUpPosition = candidateX;
                foundCatchUpPosition = true;
                break;
            }
        }
    }

    if (!foundCatchUpPosition) return;

    mWarrior->setPosition({ catchUpPosition, warriorY });
    mWarrior->setVelocity({ 0.0f, 0.0f });
    mWarrior->setMovement({ 0.0f, 0.0f });
    mWarrior->setEntityType(NPC);
    mWarrior->moveRight();
    mWarriorJumpTime = 0.0f;
    mWarriorDamageApplied = false;
    setWarriorAnimation(WARRIOR_IDLE);
}

bool LevelB::hasWarriorJumpClearance() const
{
    const Vector2 position = mWarrior->getPosition();
    const Vector2 collider = mWarrior->getColliderDimensions();
    const float topY = position.y - collider.y / 2.0f;
    const float sideOffset = collider.x / 2.0f;
    const float sampleSpacing = LEVEL_B_TILE_SIZE / 2.0f;
    const float jumpHeight = 140.0f;
    float xOverlap = 0.0f;
    float yOverlap = 0.0f;

    for (float height = sampleSpacing; height <= jumpHeight;
         height += sampleSpacing)
    {
        const float sampleY = topY - height;
        if (mGameState.map->isSolidTileAt(
                { position.x - sideOffset, sampleY },
                &xOverlap,
                &yOverlap
            ) ||
            mGameState.map->isSolidTileAt(
                { position.x, sampleY },
                &xOverlap,
                &yOverlap
            ) ||
            mGameState.map->isSolidTileAt(
                { position.x + sideOffset, sampleY },
                &xOverlap,
                &yOverlap
            ))
            return false;
    }

    return true;
}

void LevelB::setWarriorAnimation(WarriorAnimation animation)
{
    if (mWarriorAnimation == animation) return;

    if (animation == WARRIOR_ATTACK_ONE ||
        animation == WARRIOR_ATTACK_TWO)
        mWarriorDamageApplied = false;

    mWarriorAnimation = animation;
    mWarriorAnimationFrame = 0;
    mWarriorAnimationTime = 0.0f;
    mWarriorAnimationFinished = false;
}

LevelB::WarriorAnimation LevelB::getWarriorMovementAnimation() const
{
    const float verticalVelocity = mWarrior->getVelocity().y;
    if (verticalVelocity < -1.0f) return WARRIOR_JUMP;
    if (verticalVelocity > 1.0f) return WARRIOR_FALL;
    if (GetLength(mWarrior->getMovement()) > 0.0f) return WARRIOR_RUN;

    return WARRIOR_IDLE;
}

Texture2D LevelB::getWarriorAnimationTexture() const
{
    switch (mWarriorAnimation)
    {
    case WARRIOR_RUN: return mWarriorRun;
    case WARRIOR_JUMP: return mWarriorJump;
    case WARRIOR_FALL: return mWarriorFall;
    case WARRIOR_ATTACK_ONE: return mWarriorAttackOne;
    case WARRIOR_ATTACK_TWO: return mWarriorAttackTwo;
    case WARRIOR_IDLE:
    default: return mWarriorIdle;
    }
}

void LevelB::animateWarrior(float deltaTime)
{
    const Texture2D texture = getWarriorAnimationTexture();
    const int frameCount = texture.width / texture.height;
    const float secondsPerFrame =
        1.0f / static_cast<float>(Entity::DEFAULT_FRAME_SPEED);

    mWarriorAnimationTime += deltaTime;
    while (mWarriorAnimationTime >= secondsPerFrame)
    {
        mWarriorAnimationTime -= secondsPerFrame;
        if (mWarriorAnimationFrame + 1 < frameCount)
            mWarriorAnimationFrame++;
        else if (mWarriorAnimation == WARRIOR_ATTACK_ONE ||
                 mWarriorAnimation == WARRIOR_ATTACK_TWO)
        {
            mWarriorAnimationFinished = true;
            break;
        }
        else
            mWarriorAnimationFrame = 0;
    }
}

void LevelB::renderWarrior()
{
    drawWarriorFrame(
        getWarriorAnimationTexture(),
        mWarriorAnimationFrame,
        mWarrior->getPosition(),
        mWarrior->getDirection()
    );
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
    drawAtGround(mSmallTent, mapLeft + 18.0f * LEVEL_B_TILE_SIZE, groundY, 1.3f);
    drawAtGround(mGreenBirch, mapLeft + 28.0f * LEVEL_B_TILE_SIZE, groundY, 1.5f);

    drawAtlasItemAtGround(
        mDecorAtlas,
        { 192.0f, 98.0f, 164.0f, 48.0f },
        mapLeft + 31.0f * LEVEL_B_TILE_SIZE,
        groundY + 17.0f,
        1.0f
    );
    drawAtGround(mAngelStatue, mapLeft + 48.0f * LEVEL_B_TILE_SIZE, groundY, 1.3f);
    drawAtGround(mSmallTent, mapLeft + 57.0f * LEVEL_B_TILE_SIZE, groundY, 1.3f);
    drawAtlasItemAtGround(
        mDecorAtlas,
        { 315.0f, 134.0f, 48.0f, 42.0f },
        mapLeft + 68.0f * LEVEL_B_TILE_SIZE,
        groundY,
        1.4f
    );
    drawAtGround(mRedBirch, mapLeft + 73.0f * LEVEL_B_TILE_SIZE, groundY, 1.5f);
    drawAtGround(mBlueBirch, mapLeft + 96.0f * LEVEL_B_TILE_SIZE, groundY, 1.5f);
    drawAtGround(mGreenBirch, mapLeft + 104.0f * LEVEL_B_TILE_SIZE, groundY, 1.5f);

    drawAtlasItemAtGround(
        mDecorAtlas,
        { 387.0f, 265.0f, 25.0f, 55.0f },
        mGameState.map->getRightBoundary() - LEVEL_B_TILE_SIZE,
        groundY,
        1.5f
    );

    renderWarrior();
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
    delete mWarrior;
    mWarrior = nullptr;
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
    UnloadTexture(mWarriorIdle);
    mWarriorIdle = {};
    UnloadTexture(mWarriorRun);
    mWarriorRun = {};
    UnloadTexture(mWarriorJump);
    mWarriorJump = {};
    UnloadTexture(mWarriorFall);
    mWarriorFall = {};
    UnloadTexture(mWarriorAttackOne);
    mWarriorAttackOne = {};
    UnloadTexture(mWarriorAttackTwo);
    mWarriorAttackTwo = {};

    UnloadMusicStream(mGameState.bgm);
    mGameState.bgm = {};
    UnloadSound(mGameState.jumpSound);
    mGameState.jumpSound = {};
    mGameState.nextSceneID = NO_SCENE;
}
