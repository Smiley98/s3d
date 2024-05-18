#pragma once
#include "Math.h"
#include <vector>
#include <algorithm>

// Must pass pixel by reference otherwise vector copy-constructor is called.
using Pixels = std::vector<Color>;

struct Image
{
    Pixels pixels;
    int width = 0;
    int height = 0;
};

inline Color GetPixel(Image& image, int x, int y)
{
    return image.pixels[y * image.width + x];
}

inline void SetPixel(Image& image, int x, int y, Color color)
{
    image.pixels[y * image.width + x] = color;
}

inline void SetRow(Image& image, int row, Color color)
{
    for (size_t i = 0; i < image.width; i++)
        SetPixel(image, i, row, color);
}

inline void SetCol(Image& image, int col, Color color)
{
    for (size_t i = 0; i < image.height; i++)
        SetPixel(image, col, i, color);
}

void Flip(Image& image);
void LoadImage(Image& image, const char* path, bool flip = false);

inline void LoadImage(Image& image, int width, int height)
{
    image.pixels.resize(width * height);
    image.width = width;
    image.height = height;
}

inline void UnloadImage(Image& image)
{
    image.pixels.clear();
    image.width = 0;
    image.height = 0;
}

inline void Fill(Image& image, Color color)
{
    std::fill(image.pixels.begin(), image.pixels.end(), color);
}
