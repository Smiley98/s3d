#pragma once
#include "Math.h"
#include "Config.h"
#include <vector>
#include <algorithm>

struct Image
{
	std::vector<Color> pixels;
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
