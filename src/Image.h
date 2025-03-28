#pragma once
#include "Color.h"
#include <vector>

constexpr int CPU_IMAGE_SIZE = 512;

struct Image
{
	std::vector<Color> pixels;
    std::vector<float> depth;
    int width = -1;
    int height = -1;
};

void CreateImageFromFile(Image* image, const char* path, bool flip = false/*defaulted to true so students can ignore it*/);
void CreateImageFromMemory(Image* image, int width, int height, Color* pixels = nullptr);
void DestroyImage(Image* image);
void Flip(Image* image);

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
