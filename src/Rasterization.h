#pragma once
#include "Image.h"
#include "Mesh.h"

struct Rect
{
	int xMin;
	int xMax;
	int yMin;
	int yMax;
};

inline void DrawLineX(Image* image, int row, int x0, int x1, Color color)
{
	// for x in x0 to x1:
	//		set pixel at x, row
}

inline void DrawLineY(Image* image, int col, int y0, int y1, Color color)
{
	// for y in y0 to y1:
	//		set pixel at col, y
}

inline void DrawLine(Image* image, int x0, int y0, int x1, int y1, Color color)
{
	// let dx = x1 - x0
	// let dy = y1 - y0

	// let steps = absolute value of dx if its greater than that of dy, otherwise dy

	// let xStep = dx / steps as float
	// let yStep = dy / steps as float

	// x = x0
	// y = y0
	// set pixel at x, y
	// for i in steps:
	//		x += xStep
	//		y += yStep
	//		set pixel at x, y
}

inline void DrawRect(Image* image, int x, int y, int w, int h, Color color)
{
	// Option 1:
	// for ry in y to y + h:
	//		for rx in x to x + h:
	//			set pixel at rx, ry
	
	// Option 2:
	// Render vertical lines for all columns

	// Option 3:
	// Render horizontal lines for all rows
}

inline void DrawRectLines(Image* image, int x, int y, int w, int h, Color color)
{
	// Render vertical lines at x and x + w
	// Render horizontal lines at y and y + h
}

inline void DrawCircle(Image* image, int cx, int cy, int cr, Color color)
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

inline void DrawCircleLines(Image* image, int cx, int cy, int cr, Color color)
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
