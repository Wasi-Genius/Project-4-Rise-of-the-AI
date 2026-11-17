#include "cs3113.h"

Color ColorFromHex(const char *hex)
{
    // Skip the optional leading marker
    if (hex[0] == '#') hex++;

    // Use full opacity when no alpha value is provided
    unsigned int r = 0,
                 g = 0,
                 b = 0,
                 a = 255;

    // Read a six digit RGB color
    if (sscanf(hex, "%02x%02x%02x", &r, &g, &b) == 3)
    {
        return Color{ static_cast<unsigned char>(r),
                      static_cast<unsigned char>(g),
                      static_cast<unsigned char>(b),
                      static_cast<unsigned char>(a) };
    }

    // Read an eight digit RGBA color
    if (sscanf(hex, "%02x%02x%02x%02x", &r, &g, &b, &a) == 4)
    {
        return Color{ static_cast<unsigned char>(r),
                      static_cast<unsigned char>(g),
                      static_cast<unsigned char>(b),
                      static_cast<unsigned char>(a) };
    }

    // Use white when the color string is invalid
    return RAYWHITE;
}

// Return the length of a vector
float GetLength(const Vector2 vector)
{
    return sqrtf(
        pow(vector.x, 2) + pow(vector.y, 2)
    );
}

// Change a vector to unit length while keeping its direction
void Normalise(Vector2 *vector)
{
    float magnitude = GetLength(*vector);

    vector->x /= magnitude;
    vector->y /= magnitude;
}

// Find one cell in a texture atlas
Rectangle getUVRectangle(const Texture2D *texture, int index, int rows, int cols)
{
    // Calculate the source rectangle for the selected atlas cell
    float uCoord = static_cast<float>(index % cols) / static_cast<float>(cols);
    uCoord *= texture->width;

    float vCoord = static_cast<float>(index / cols) / static_cast<float>(rows);
    vCoord *= texture->height;

    float sliceWidth  = texture->width  / static_cast<float>(cols);
    float sliceHeight = texture->height / static_cast<float>(rows);

    return {
        uCoord,
        vCoord,
        sliceWidth,
        sliceHeight
    };
}
