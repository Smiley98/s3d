#pragma once
#include "Math.h"
#include "Config.h"
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

inline void DrawLineX(Image& image, int row, int x0, int x1, Color color)
{
	for (int x = x0; x <= x1; x++)
		SetPixel(image, x, row, color);
}

inline void DrawLineY(Image& image, int col, int y0, int y1, Color color)
{
	for (int y = y0; y <= y1; y++)
		SetPixel(image, col, y, color);
}

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

inline void DrawCircle(Image& image, int cx, int cy, int cr, Color color)
{
	int x = 0;
	int y = cr;
	int d = 3 - 2 * cr;

	auto line = [&](int lx, int ly, int l)
	{
		for (int dx = -l; dx <= l; dx++)
			SetPixel(image, lx + dx, ly, color);
	};

	while (y >= x)
	{
		line(cx, cy + y, x);
		line(cx, cy - y, x);
		line(cx, cy + x, y);
		line(cx, cy - x, y);

		x++;
		if (d > 0)
		{
			y--;
			d = d + 4 * (x - y) + 10;
		}
		else
		{
			d = d + 4 * x + 6;
		}
	}
}

inline void DrawRectLines(Image& image, int x, int y, int w, int h, Color color)
{
	DrawLineX(image, y + 0, x, x + w, color);
	DrawLineX(image, y + h, x, x + w, color);
	DrawLineY(image, x + 0, y, y + h, color);
	DrawLineY(image, x + w, y, y + h, color);
}

inline void DrawCircleLines(Image& image, int cx, int cy, int cr, Color color)
{
	int x = 0;
	int y = cr;
	int d = 3 - 2 * cr;

	while (y >= x)
	{
		SetPixel(image, cx + x, cy + y, color);
		SetPixel(image, cx - x, cy + y, color);
		SetPixel(image, cx + x, cy - y, color);
		SetPixel(image, cx - x, cy - y, color);
		SetPixel(image, cx + y, cy + x, color);
		SetPixel(image, cx - y, cy + x, color);
		SetPixel(image, cx + y, cy - x, color);
		SetPixel(image, cx - y, cy - x, color);

		x++;
		if (d > 0)
		{
			y--;
			d = d + 4 * (x - y) + 10;
		}
		else
		{
			d = d + 4 * x + 6;
		}
	}
}

/////////////////////////////////////////////////////////////////////
// ********************** Conversion Functions **********************
/////////////////////////////////////////////////////////////////////

inline void ScreenToImage(Image& image, int* sx, int* sy)
{
	*sx *= image.width / SCREEN_WIDTH;
	*sy *= image.height / SCREEN_HEIGHT;
}

inline void ImageToScreen(Image& image, int* ix, int* iy)
{
	*ix *= SCREEN_WIDTH / image.width;
	*iy *= SCREEN_HEIGHT / image.height;
}

inline Vector2 ScreenToImage(Image& image, Vector2 scr)
{
	Vector2 img;
	img.x = scr.x * (image.width / (float)SCREEN_WIDTH);
	img.y = scr.y * (image.height / (float)SCREEN_HEIGHT);
	return img;
}

inline Vector2 ImageToScreen(Image& image, Vector2 img)
{
	Vector2 scr;
	scr.x = img.x * (SCREEN_WIDTH / (float)image.width);
	scr.y = img.y * (SCREEN_HEIGHT / (float)image.height);
	return scr;
}
