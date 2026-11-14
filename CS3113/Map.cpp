#include "Map.h"

Map::Map(
    int           mapColumns,
    int           mapRows,
    unsigned int *levelData,
    const char   *textureFilePath,
    float         tileSize,
    int           textureColumns,
    int           textureRows,
    Vector2       origin
)
    : mMapColumns{ mapColumns }, mMapRows{ mapRows }, mLevelData{ levelData },
      mTextureAtlas{ LoadTexture(textureFilePath) }, mTileSize{ tileSize },
      mTextureColumns{ textureColumns }, mTextureRows{ textureRows },
      mOrigin{ origin }, mLeftBoundary{ 0.0f }, mRightBoundary{ 0.0f },
      mTopBoundary{ 0.0f }, mBottomBoundary{ 0.0f }
{
    build();
}

Map::~Map()
{
    UnloadTexture(mTextureAtlas);
}

void Map::build()
{
    // The origin is the map's centre, so subtracting half its pixel dimensions
    // gives the top-left world coordinate used by both drawing and collision.
    mLeftBoundary = mOrigin.x - mMapColumns * mTileSize / 2.0f;
    mRightBoundary = mLeftBoundary + mMapColumns * mTileSize;
    mTopBoundary = mOrigin.y - mMapRows * mTileSize / 2.0f;
    mBottomBoundary = mTopBoundary + mMapRows * mTileSize;

    mTextureAreas.clear();

    if (mMapColumns <= 0 || mMapRows <= 0 || mLevelData == nullptr ||
        mTileSize <= 0.0f || mTextureColumns <= 0 || mTextureRows <= 0 ||
        mTextureAtlas.width <= 0 || mTextureAtlas.height <= 0)
    {
        LOG("Cannot build map: map or texture atlas dimensions are invalid.");
        return;
    }

    // Precompute each atlas cell's source rectangle. Tile IDs in level data
    // start at 1, so render() converts them to zero-based vector indices.
    const int atlasTileCount = mTextureColumns * mTextureRows;
    for (int tileIndex = 0; tileIndex < atlasTileCount; tileIndex++)
    {
        mTextureAreas.push_back(getUVRectangle(
            &mTextureAtlas, tileIndex, mTextureRows, mTextureColumns
        ));
    }
}

void Map::render()
{
    if (mLevelData == nullptr || mTextureAreas.empty()) return;

    // The level array is row-major: each consecutive group of mapColumns
    // values describes one horizontal row. Zero is the empty-cell sentinel.
    for (int row = 0; row < mMapRows; row++)
    {
        for (int column = 0; column < mMapColumns; column++)
        {
            const unsigned int tileID =
                mLevelData[row * mMapColumns + column];

            if (tileID == 0) continue;

            if (tileID > mTextureAreas.size())
            {
                LOG("Cannot render map tile: tile ID is outside the atlas.");
                continue;
            }

            const Rectangle source = mTextureAreas[tileID - 1];
            const Rectangle destination = {
                mLeftBoundary + column * mTileSize,
                mTopBoundary + row * mTileSize,
                mTileSize,
                mTileSize
            };

            // DrawTexturePro scales an atlas cell into a world-space map cell.
            DrawTexturePro(
                mTextureAtlas, source, destination, { 0.0f, 0.0f }, 0.0f, WHITE
            );
        }
    }
}

bool Map::isSolidTileAt(Vector2 position, float *xOverlap, float *yOverlap)
{
    if (xOverlap == nullptr || yOverlap == nullptr) return false;

    *xOverlap = 0.0f;
    *yOverlap = 0.0f;

    // Points outside the map are empty. This also prevents invalid row/column
    // indices when the player walks or falls past an edge.
    if (position.x < mLeftBoundary || position.x >= mRightBoundary ||
        position.y < mTopBoundary || position.y >= mBottomBoundary ||
        mLevelData == nullptr || mTileSize <= 0.0f)
    {
        return false;
    }

    const int column = static_cast<int>(
        (position.x - mLeftBoundary) / mTileSize
    );
    const int row = static_cast<int>(
        (position.y - mTopBoundary) / mTileSize
    );
    const unsigned int tileID = mLevelData[row * mMapColumns + column];

    // Empty cells do not block movement; any non-zero map ID is a solid tile.
    if (tileID == 0) return false;

    const float tileLeft = mLeftBoundary + column * mTileSize;
    const float tileTop = mTopBoundary + row * mTileSize;

    // These penetration distances let Entity move its probe back to the
    // nearest tile edge when resolving a collision.
    *xOverlap = (position.x - tileLeft < mTileSize / 2.0f)
        ? position.x - tileLeft
        : tileLeft + mTileSize - position.x;
    *yOverlap = (position.y - tileTop < mTileSize / 2.0f)
        ? position.y - tileTop
        : tileTop + mTileSize - position.y;

    return true;
}
