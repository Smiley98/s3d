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
	int dx = x1 - x0;
	int dy = y1 - y0;

	int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);
	float xStep = dx / (float)steps;
	float yStep = dy / (float)steps;

	float x = x0;
	float y = y0;
	for (int i = 0; i < steps; i++)
	{
		x += xStep;
		y += yStep;
		SetPixel(image, x, y, color);
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

inline void DrawTriangle(Image& image, Vector3 v0, Vector3 v1, Vector3 v2, Color color)
{
	// Determine triangle's AABB
	int xMin = image.width - 1;
	int yMin = image.height - 1;
	int xMax = 0;
	int yMax = 0;

	Vector3 vertices[]{ v0, v1, v2 };
	for (int i = 0; i < 3; i++)
	{
		const int x = vertices[i].x;
		const int y = vertices[i].y;
		xMin = std::max(0, std::min(xMin, x));
		yMin = std::max(0, std::min(xMin, y));
		xMax = std::min(image.width - 1, std::max(xMax, x));
		yMax = std::min(image.height - 1, std::max(yMax, y));
	}

	// Loop through every pixel in triangle's AABB.
	// Discard if barycentric coordinates are negative (point not in triangle)!
	for (int x = xMin; x < xMax; x++)
	{
		for (int y = yMin; y < yMax; y++)
		{
			Vector3 p{ x, y, 0.0f };
			Vector3 bc = Barycenter(p, v0, v1, v2);
			if (bc.x < 0.0f || bc.y < 0.0f || bc.z < 0.0f)
				continue;
			SetPixel(image, x, y, color);
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

/////////////////////////////////////////////////////////////////////
// ********************** Collision Functions ***********************
/////////////////////////////////////////////////////////////////////

inline bool Overlaps(int min1, int max1, int min2, int max2)
{
	return !((max1 < min2) || (max2 < min1));
}

inline bool RectRect(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
	int xMin1 = x1;
	int xMin2 = x2;
	int yMin1 = y1;
	int yMin2 = y2;

	int xMax1 = x1 + w1;
	int xMax2 = x2 + w2;
	int yMax1 = y1 + h1;
	int yMax2 = y2 + h2;

	bool collision =
		Overlaps(xMin1, xMax1, xMin2, xMax2) &&
		Overlaps(yMin1, yMax1, yMin2, yMax2);
	return collision;
}
