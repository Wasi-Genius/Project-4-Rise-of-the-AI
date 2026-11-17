#include "Entity.h"

Entity::Entity() :
    mPosition{ 0.0f, 0.0f }, mMovement{ 0.0f, 0.0f },
    mVelocity{ 0.0f, 0.0f }, mAcceleration{ 0.0f, 0.0f },
    mScale{ DEFAULT_SIZE, DEFAULT_SIZE },
    mColliderDimensions{ DEFAULT_SIZE, DEFAULT_SIZE },
    mTexture{}, mTextureType{ SINGLE }, mSpriteSheetDimensions{},
    mAnimationAtlas{}, mAnimationIndices{}, mDirection{ RIGHT },
    mFrameSpeed{ 0 }, mSpeed{ DEFAULT_SPEED }, mAngle{ 0.0f },
    mEntityType{ NONE }, mAIType{ WANDERER }, mAIState{ IDLE } {}

Entity::Entity(
    Vector2     position,
    Vector2     scale,
    const char *textureFilepath,
    EntityType  entityType
) :
    mPosition{ position }, mMovement{ 0.0f, 0.0f },
    mVelocity{ 0.0f, 0.0f }, mAcceleration{ 0.0f, 0.0f },
    mScale{ scale }, mColliderDimensions{ scale },
    mTexture{ LoadTexture(textureFilepath) }, mTextureType{ SINGLE },
    mSpriteSheetDimensions{}, mAnimationAtlas{}, mAnimationIndices{},
    mDirection{ RIGHT }, mFrameSpeed{ 0 }, mSpeed{ DEFAULT_SPEED },
    mAngle{ 0.0f }, mEntityType{ entityType }, mAIType{ WANDERER },
    mAIState{ IDLE } {}

Entity::Entity(
    Vector2                               position,
    Vector2                               scale,
    const char                           *textureFilepath,
    TextureType                           textureType,
    Vector2                               spriteSheetDimensions,
    std::map<Direction, std::vector<int>> animationAtlas,
    EntityType                            entityType
) :
    mPosition{ position }, mMovement{ 0.0f, 0.0f },
    mVelocity{ 0.0f, 0.0f }, mAcceleration{ 0.0f, 0.0f },
    mScale{ scale }, mColliderDimensions{ scale },
    mTexture{ LoadTexture(textureFilepath) }, mTextureType{ textureType },
    mSpriteSheetDimensions{ spriteSheetDimensions },
    mAnimationAtlas{ animationAtlas },
    mAnimationIndices{ animationAtlas.at(RIGHT) }, mDirection{ RIGHT },
    mFrameSpeed{ DEFAULT_FRAME_SPEED }, mSpeed{ DEFAULT_SPEED },
    mAngle{ 0.0f }, mEntityType{ entityType }, mAIType{ WANDERER },
    mAIState{ IDLE } {}

Entity::~Entity()
{
    UnloadTexture(mTexture);
    UnloadTexture(mIdleTexture);
    UnloadTexture(mTurnAroundTexture);
}

void Entity::setTurnAroundAnimation(
    const char *textureFilepath,
    Vector2 spriteSheetDimensions,
    std::vector<int> animationFrames
)
{
    UnloadTexture(mTurnAroundTexture);
    mTurnAroundTexture = LoadTexture(textureFilepath);
    mTurnAroundSpriteSheetDimensions = spriteSheetDimensions;
    mTurnAroundAnimationFrames = animationFrames;
    mFlipWithDirection = true;
}

void Entity::setIdleAnimation(
    const char *textureFilepath,
    Vector2 spriteSheetDimensions,
    std::vector<int> animationFrames,
    int frameSpeed
)
{
    UnloadTexture(mIdleTexture);
    mIdleTexture = LoadTexture(textureFilepath);
    mIdleSpriteSheetDimensions = spriteSheetDimensions;
    mIdleAnimationFrames = animationFrames;
    mIdleFrameSpeed = frameSpeed > 0 ? frameSpeed : DEFAULT_FRAME_SPEED;
    mIdleAnimationFrame = 0;
    mIdleAnimationTime = 0.0f;
    mIsIdleAnimating = false;
}

void Entity::startTurnAround()
{
    if (mTurnAroundAnimationFrames.empty()) return;

    mIsTurningAround = true;
    mTurnAroundFrameIndex = 0;
    mTurnAroundAnimationTime = 0.0f;
}

// Resolve vertical collisions with other entities
void Entity::checkCollisionY(
    Entity *collidableEntities, int collisionCheckCount
)
{
    for (int i = 0; i < collisionCheckCount; i++)
    {
        // Check each entity in the collision list
        Entity *collidableEntity = &collidableEntities[i];

        if (isColliding(collidableEntity))
        {
            const Vector2 position = getColliderPosition();
            const Vector2 otherPosition =
                collidableEntity->getColliderPosition();

            // Find the vertical overlap between the colliders
            float yDistance = fabs(position.y - otherPosition.y);
            float yOverlap  = fabs(
                yDistance - (mColliderDimensions.y / 2.0f) -
                (collidableEntity->mColliderDimensions.y / 2.0f)
            );

            // Move out of the collision and stop vertical motion
            if (mVelocity.y > 0)
            {
                mPosition.y        -= yOverlap;
                mVelocity.y         = 0;
                mIsCollidingBottom  = true;
            }
            else if (mVelocity.y < 0)
            {
                mPosition.y     += yOverlap;
                mVelocity.y      = 0;
                mIsCollidingTop  = true;
            }
        }
    }
}

void Entity::checkCollisionX(
    Entity *collidableEntities, int collisionCheckCount
)
{
    for (int i = 0; i < collisionCheckCount; i++)
    {
        Entity *collidableEntity = &collidableEntities[i];

        if (isColliding(collidableEntity))
        {
            const Vector2 position = getColliderPosition();
            const Vector2 otherPosition =
                collidableEntity->getColliderPosition();

            // Ignore small vertical overlap while standing on a platform
            float yDistance = fabs(position.y - otherPosition.y);
            float yOverlap  = fabs(
                yDistance - (mColliderDimensions.y / 2.0f) -
                (collidableEntity->mColliderDimensions.y / 2.0f)
            );

            // Skip horizontal resolution when barely touching vertically
            if (yOverlap < Y_COLLISION_THRESHOLD) continue;

            float xDistance = fabs(position.x - otherPosition.x);
            float xOverlap  = fabs(
                xDistance - (mColliderDimensions.x / 2.0f) -
                (collidableEntity->mColliderDimensions.x / 2.0f)
            );

            if (mVelocity.x > 0)
            {
                mPosition.x -= xOverlap;
                mVelocity.x  = 0;

                mIsCollidingRight = true;
            }
            else if (mVelocity.x < 0)
            {
                mPosition.x += xOverlap;
                mVelocity.x  = 0;

                mIsCollidingLeft = true;
            }
        }
    }
}

void Entity::checkCollisionY(Map *map)
{
    if (map == nullptr) return;

    const Vector2 position = getColliderPosition();
    Vector2 topCentreProbe = {
        position.x, position.y - (mColliderDimensions.y / 2.0f)
    };
    Vector2 topLeftProbe = {
        position.x - (mColliderDimensions.x / 2.0f),
        position.y - (mColliderDimensions.y / 2.0f)
    };
    Vector2 topRightProbe = {
        position.x + (mColliderDimensions.x / 2.0f),
        position.y - (mColliderDimensions.y / 2.0f)
    };

    Vector2 bottomCentreProbe = {
        position.x, position.y + (mColliderDimensions.y / 2.0f)
    };
    Vector2 bottomLeftProbe = {
        position.x - (mColliderDimensions.x / 2.0f),
        position.y + (mColliderDimensions.y / 2.0f)
    };
    Vector2 bottomRightProbe = {
        position.x + (mColliderDimensions.x / 2.0f),
        position.y + (mColliderDimensions.y / 2.0f)
    };

    float xOverlap = 0.0f;
    float yOverlap = 0.0f;

    // Stop upward motion at a solid tile
    if ((map->isSolidTileAt(topCentreProbe, &xOverlap, &yOverlap) ||
         map->isSolidTileAt(topLeftProbe, &xOverlap, &yOverlap) ||
         map->isSolidTileAt(topRightProbe, &xOverlap, &yOverlap)) &&
        mVelocity.y < 0.0f)
    {
        mPosition.y     += yOverlap; // push down
        mVelocity.y      = 0.0f;
        mIsCollidingTop  = true;
    }

    // Stop downward motion at a solid tile
    if ((map->isSolidTileAt(bottomCentreProbe, &xOverlap, &yOverlap) ||
         map->isSolidTileAt(bottomLeftProbe, &xOverlap, &yOverlap) ||
         map->isSolidTileAt(bottomRightProbe, &xOverlap, &yOverlap)) &&
        mVelocity.y > 0.0f)
    {
        mPosition.y        -= yOverlap; // push up
        mVelocity.y         = 0.0f;
        mIsCollidingBottom  = true;
    }
}

void Entity::checkCollisionX(Map *map)
{
    if (map == nullptr) return;

    const Vector2 position = getColliderPosition();
    Vector2 leftCentreProbe = {
        position.x - (mColliderDimensions.x / 2.0f), position.y
    };
    Vector2 leftTopProbe = {
        position.x - (mColliderDimensions.x / 2.0f),
        position.y - (mColliderDimensions.y / 2.0f)
    };
    Vector2 leftBottomProbe = {
        position.x - (mColliderDimensions.x / 2.0f),
        position.y + (mColliderDimensions.y / 2.0f)
    };

    Vector2 rightCentreProbe = {
        position.x + (mColliderDimensions.x / 2.0f), position.y
    };
    Vector2 rightTopProbe = {
        position.x + (mColliderDimensions.x / 2.0f),
        position.y - (mColliderDimensions.y / 2.0f)
    };
    Vector2 rightBottomProbe = {
        position.x + (mColliderDimensions.x / 2.0f),
        position.y + (mColliderDimensions.y / 2.0f)
    };

    float xOverlap = 0.0f;
    float yOverlap = 0.0f;

    // Stop rightward motion at a solid tile
    if ((map->isSolidTileAt(rightCentreProbe, &xOverlap, &yOverlap) ||
         map->isSolidTileAt(rightTopProbe, &xOverlap, &yOverlap) ||
         map->isSolidTileAt(rightBottomProbe, &xOverlap, &yOverlap)) &&
        mVelocity.x > 0.0f && yOverlap >= 0.5f)
    {
        mPosition.x       -= xOverlap * 1.01f;
        mVelocity.x        = 0.0f;
        mIsCollidingRight  = true;
    }

    // Stop leftward motion at a solid tile
    if ((map->isSolidTileAt(leftCentreProbe, &xOverlap, &yOverlap) ||
         map->isSolidTileAt(leftTopProbe, &xOverlap, &yOverlap) ||
         map->isSolidTileAt(leftBottomProbe, &xOverlap, &yOverlap)) &&
        mVelocity.x < 0.0f && yOverlap >= 0.5f)
    {
        mPosition.x      += xOverlap * 1.01f;
        mVelocity.x       = 0.0f;
        mIsCollidingLeft  = true;
    }
}

// Check whether two active entities overlap
bool Entity::isColliding(Entity *other) const
{
    if (!other->isActive() || other == this) return false;

    const Vector2 position = getColliderPosition();
    const Vector2 otherPosition = other->getColliderPosition();
    float xDistance = fabs(position.x - otherPosition.x) -
        ((mColliderDimensions.x + other->getColliderDimensions().x) / 2.0f);
    float yDistance = fabs(position.y - otherPosition.y) -
        ((mColliderDimensions.y + other->getColliderDimensions().y) / 2.0f);

    return xDistance < 0.0f && yDistance < 0.0f;
}

// Advance the current movement animation
void Entity::animate(float deltaTime)
{
    mAnimationIndices = mAnimationAtlas.at(mDirection);

    mAnimationTime += deltaTime;
    const float secondsPerFrame = 1.0f / mFrameSpeed;

    while (mAnimationTime >= secondsPerFrame)
    {
        mAnimationTime -= secondsPerFrame;
        mCurrentFrameIndex = (mCurrentFrameIndex + 1) %
            static_cast<int>(mAnimationIndices.size());
    }
}

void Entity::animateIdle(float deltaTime)
{
    if (mIdleAnimationFrames.empty()) return;

    mIdleAnimationTime += deltaTime;
    const float secondsPerFrame = 1.0f / mIdleFrameSpeed;

    while (mIdleAnimationTime >= secondsPerFrame)
    {
        mIdleAnimationTime -= secondsPerFrame;
        mIdleAnimationFrame = (mIdleAnimationFrame + 1) %
            static_cast<int>(mIdleAnimationFrames.size());
    }
}

void Entity::animateTurnAround(float deltaTime)
{
    mTurnAroundAnimationTime += deltaTime;
    const float secondsPerFrame = 1.0f / mFrameSpeed;

    while (mTurnAroundAnimationTime >= secondsPerFrame && mIsTurningAround)
    {
        mTurnAroundAnimationTime -= secondsPerFrame;
        mTurnAroundFrameIndex++;

        if (mTurnAroundFrameIndex >=
            static_cast<int>(mTurnAroundAnimationFrames.size()))
        {
            mIsTurningAround = false;
            mTurnAroundFrameIndex = 0;
            mTurnAroundAnimationTime = 0.0f;
            mCurrentFrameIndex = 0;
            mAnimationTime = 0.0f;
        }
    }
}

void Entity::displayCollider()
{
    // Draw the entity collision box
    const Vector2 position = getColliderPosition();
    Rectangle colliderBox = {
        position.x - mColliderDimensions.x / 2.0f,
        position.y - mColliderDimensions.y / 2.0f,
        mColliderDimensions.x,
        mColliderDimensions.y
    };

    DrawRectangleLines(
        colliderBox.x,
        colliderBox.y,
        colliderBox.width,
        colliderBox.height,
        GREEN
    );
}

void Entity::AIWander()
{
    if (mIsCollidingLeft)
        moveRight();
    else if (mIsCollidingRight)
        moveLeft();
    else if (mMovement.x == 0.0f)
        moveLeft();
}

void Entity::AIFollow(Entity *target)
{
    if (target == nullptr) return;

    switch (mAIState)
    {
    case IDLE:
        if (Vector2Distance(mPosition, target->getPosition()) < 250.0f)
            mAIState = FOLLOWING;
        break;
    case WALKING:
    case FOLLOWING:
        // Face and move toward the target
        if (mPosition.x > target->getPosition().x)
            moveLeft();
        else
            moveRight();
        break;
    default:
        break;
    }
}

void Entity::AIFly(Entity *target)
{
    if (target == nullptr)
    {
        mMovement = {0.0f, 0.0f};
        return;
    }

    const Vector2 targetPosition = target->getPosition();
    const float differenceX = targetPosition.x - mPosition.x;
    const float differenceY = targetPosition.y - mPosition.y;
    const float distance = GetLength({differenceX, differenceY});

    if (distance == 0.0f)
    {
        mMovement = {0.0f, 0.0f};
        return;
    }

    mMovement = {differenceX / distance, differenceY / distance};

    if (differenceX < 0.0f)
        setDirection(LEFT);
    else if (differenceX > 0.0f)
        setDirection(RIGHT);
}

void Entity::AIActivate(Entity *target)
{
    switch (mAIType)
    {
    case WANDERER:
        AIWander();
        break;
    case FOLLOWER:
        AIFollow(target);
        break;
    case FLYER:
        AIFly(target);
        break;
    default:
        break;
    }
}

void Entity::update(
    float   deltaTime,
    Entity *player,
    Map    *map,
    Entity *collidableEntities,
    int     collisionCheckCount
)
{
    if (mEntityStatus == INACTIVE) return;

    if (mEntityType == NPC) AIActivate(player);

    resetColliderFlags();

    // Flyers ignore gravity and solid map tiles
    if (mAIType == FLYER)
    {
        mVelocity = {
            mMovement.x * mSpeed,
            mMovement.y * mSpeed
        };
        mPosition.x += mVelocity.x * deltaTime;
        mPosition.y += mVelocity.y * deltaTime;
    }
    else
    {
        mVelocity.x = mMovement.x * mSpeed;

        mVelocity.x += mAcceleration.x * deltaTime;
        mVelocity.y += mAcceleration.y * deltaTime;

        // Apply a requested jump
        if (mIsJumping)
        {
            mIsJumping = false;
            mVelocity.y -= mJumpingPower;
        }

        mPosition.y += mVelocity.y * deltaTime;
        checkCollisionY(collidableEntities, collisionCheckCount);
        checkCollisionY(map);

        mPosition.x += mVelocity.x * deltaTime;
        checkCollisionX(collidableEntities, collisionCheckCount);
        checkCollisionX(map);
    }

    const bool shouldAnimateIdle =
        mTextureType == ATLAS &&
        !mIdleAnimationFrames.empty() &&
        GetLength(mMovement) == 0.0f &&
        mIsCollidingBottom;
    if (shouldAnimateIdle != mIsIdleAnimating)
    {
        mIsIdleAnimating = shouldAnimateIdle;
        mIdleAnimationFrame = 0;
        mIdleAnimationTime = 0.0f;

        if (!mIsIdleAnimating)
        {
            mCurrentFrameIndex = 0;
            mAnimationTime = 0.0f;
        }
    }

    if (mIsTurningAround)
        animateTurnAround(deltaTime);
    else if (mIsIdleAnimating)
        animateIdle(deltaTime);
    else if (mTextureType == ATLAS && GetLength(mMovement) != 0 &&
             mIsCollidingBottom)
        animate(deltaTime);
}

void Entity::render()
{
    if (mEntityStatus == INACTIVE) return;

    Rectangle textureArea;
    Texture2D texture = mTexture;

    if (mIsTurningAround)
    {
        texture = mTurnAroundTexture;
        textureArea = getUVRectangle(
            &texture,
            mTurnAroundAnimationFrames[mTurnAroundFrameIndex],
            mTurnAroundSpriteSheetDimensions.x,
            mTurnAroundSpriteSheetDimensions.y
        );
    }
    else switch (mTextureType)
    {
    case SINGLE:
        // Use the full texture for a single image
        textureArea = {
            0.0f, 0.0f,
            static_cast<float>(mTexture.width),
            static_cast<float>(mTexture.height)
        };
        break;
    case ATLAS:
        if (mIsIdleAnimating)
        {
            texture = mIdleTexture;
            textureArea = getUVRectangle(
                &texture,
                mIdleAnimationFrames[mIdleAnimationFrame],
                mIdleSpriteSheetDimensions.x,
                mIdleSpriteSheetDimensions.y
            );
        }
        else
            textureArea = getUVRectangle(
                &texture,
                mAnimationIndices[mCurrentFrameIndex],
                mSpriteSheetDimensions.x,
                mSpriteSheetDimensions.y
            );
        break;
    default:
        break;
    }

    if (mFlipWithDirection && mDirection == LEFT)
        textureArea.width *= -1.0f;

    // Center the sprite on the entity position
    Rectangle destinationArea = {
        mPosition.x,
        mPosition.y,
        static_cast<float>(mScale.x),
        static_cast<float>(mScale.y)
    };

    // Rotate and flip around the sprite center
    Vector2 originOffset = {
        static_cast<float>(mScale.x) / 2.0f,
        static_cast<float>(mScale.y) / 2.0f
    };

    DrawTexturePro(
        texture,
        textureArea, destinationArea, originOffset,
        mAngle, WHITE
    );

    // Call displayCollider to show the collision box
}
