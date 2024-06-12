#pragma once
#include "Image.h"
#include "Config.h"

/////////////////////////////////////////////////////////////////////
// ********************** Conversion Functions **********************
/////////////////////////////////////////////////////////////////////

inline void ScreenToImage(const Image& image, int* sx, int* sy)
{
	*sx *= image.width / SCREEN_WIDTH;
	*sy *= image.height / SCREEN_HEIGHT;
}

inline void ImageToScreen(const Image& image, int* ix, int* iy)
{
	*ix *= SCREEN_WIDTH / image.width;
	*iy *= SCREEN_HEIGHT / image.height;
}

inline Vector2 ScreenToImage(const Image& image, Vector2 scr)
{
	Vector2 img;
	img.x = scr.x * (image.width / (float)SCREEN_WIDTH);
	img.y = scr.y * (image.height / (float)SCREEN_HEIGHT);
	return img;
}

inline Vector2 ImageToScreen(const Image& image, Vector2 img)
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

/////////////////////////////////////////////////////////////////////
// ************************* Miscellaneous **************************
/////////////////////////////////////////////////////////////////////
inline Vector3 Palette(Vector3 a, Vector3 b, Vector3 c, Vector3 d, float t)
{
	Vector3 cd = (c * t + d) * 6.28318f;
	cd.x = cosf(cd.x);
	cd.y = cosf(cd.y);
	cd.z = cosf(cd.z);
	return a + b * cd;
}
