#pragma once
#include "Color.h"
#include <vector>
// See ImageUtils.h for additional features.

struct Image
{
	std::vector<Color> pixels;
    std::vector<float> depth;
    int width = 0;
    int height = 0;
};

void Flip(Image* image);
void LoadImage(Image* image, const char* path, bool flip = false);
inline void LoadImage(Image* image, int width, int height)
{
    image->pixels.resize(width * height);
    image->depth.resize(width * height);
    image->width = width;
    image->height = height;
}

inline void UnloadImage(Image* image)
{
    image->pixels.clear();
    image->depth.clear();
    image->width = 0;
    image->height = 0;
}

inline void ClearColor(Image* image, Color color)
{
    std::fill(image->pixels.begin(), image->pixels.end(), color);
}

inline void ClearDepth(Image* image, float depth)
{
    std::fill(image->depth.begin(), image->depth.end(), depth);
}

inline Color GetPixel(const Image& image, int x, int y)
{
    return image.pixels[y * image.width + x];
}

inline void SetPixel(Image* image, int x, int y, Color color)
{
    image->pixels[y * image->width + x] = color;
}

inline float GetDepth(const Image& image, int x, int y)
{
    return image.depth[y * image.width + x];
}

inline void SetDepth(Image* image, int x, int y, float depth)
{
    image->depth[y * image->width + x] = depth;
}

inline void SetRow(Image* image, int row, Color color)
{
    for (size_t i = 0; i < image->width; i++)
        SetPixel(image, i, row, color);
}

inline void SetCol(Image* image, int col, Color color)
{
    for (size_t i = 0; i < image->height; i++)
        SetPixel(image, col, i, color);
}

void CreateImages();
void DestroyImages();

extern Image gImageDiffuse;