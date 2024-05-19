#pragma once
#include "Math.h"
#include <cassert>
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

/////////////////////////////////////////////////////////////////////
// ******************** Rasterization Functions ********************
/////////////////////////////////////////////////////////////////////

inline void DrawLine(Image& image, int x0, int y0, int x1, int y1, Color color)
{
	// Make the horizontal component the largest (avoid divide by zero)!
	bool steep = false;
	if (fabsf(x0 - x1) < fabsf(y0 - y1))
	{
		steep = true;
		std::swap(x0, y0);
		std::swap(x1, y1);
	}

	// Make the line point left-to-right
	if (x0 > x1)
	{
		std::swap(x0, x1);
		std::swap(y0, y1);
	}

	for (int x = x0; x <= x1; x++)
	{
		// Calculate interpolation value
		float t = (x - x0) / (float)(x1 - x0);

		// Express y in terms of x by lerping!
		int y = y0 * (1.0f - t) + y1 * t;

		int px = steep ? y : x;
		int py = steep ? x : y;
		SetPixel(image, px, py, color);
	}
}

inline void DrawRect(Image& image, int x, int y, int w, int h, Color color)
{
	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < w; j++)
		{
			SetPixel(image, x + j, y + i, color);
		}
	}
}

