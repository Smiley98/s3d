#pragma once
#include "Image.h"
#include "Mesh.h"

inline void DrawLineX(Image* image, int row, int x0, int x1, Color color)
{
	for (int x = x0; x <= x1; x++)
		SetPixel(image, x, row, color);
}

inline void DrawLineY(Image* image, int col, int y0, int y1, Color color)
{
	for (int y = y0; y <= y1; y++)
		SetPixel(image, col, y, color);
}

inline void DrawLine(Image* image, int x0, int y0, int x1, int y1, Color color)
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

inline void DrawRect(Image* image, int x, int y, int w, int h, Color color)
{
	for (int i = 0; i < h; i++)
	{
		for (int j = 0; j < w; j++)
		{
			SetPixel(image, x + j, y + i, color);
		}
	}
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

inline void DrawRectLines(Image* image, int x, int y, int w, int h, Color color)
{
	DrawLineX(image, y + 0, x, x + w, color);
	DrawLineX(image, y + h, x, x + w, color);
	DrawLineY(image, x + 0, y, y + h, color);
	DrawLineY(image, x + w, y, y + h, color);
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

// Refactor to pass in mesh. Even with globals I still need vertex indices...
inline void DrawTriangle(Image* image, Mesh mesh, size_t face, Color color, bool depth = true)
{
	Vector3 vertices[3];	// screen-space vertices (clip space, gl_Position in the future)
	Vector3 positions[3];	// world-space positions
	Vector3 normals[3];
	Vector2 tcoords[3];
	Vector3 colors[3];

	size_t vertex = face * 3;
	for (size_t i = 0; i < 3; i++)
	{
		positions[i] = mesh.positions[vertex + i];
		vertices[i] = mesh.positions[vertex + i];
		vertices[i].x = Remap(vertices[i].x, -1.0f, 1.0f, 0, image->width - 1);
		vertices[i].y = Remap(vertices[i].y, -1.0f, 1.0f, 0, image->height - 1);
	}

	if (mesh.normals != nullptr)
	{
		for (size_t i = 0; i < 3; i++)
			normals[i] = mesh.normals[vertex + i];
	}

	if (mesh.tcoords != nullptr)
	{
		for (size_t i = 0; i < 3; i++)
			tcoords[i] = mesh.tcoords[vertex + i];
	}

	if (mesh.colors != nullptr)
	{
		for (size_t i = 0; i < 3; i++)
			colors[i] = mesh.colors[vertex + i];
	}

	// Determine triangle's AABB
	int xMin = image->width - 1;
	int yMin = image->height - 1;
	int xMax = 0;
	int yMax = 0;
	for (int i = 0; i < 3; i++)
	{
		const int x = vertices[i].x;
		const int y = vertices[i].y;
		xMin = std::max(0, std::min(xMin, x));
		yMin = std::max(0, std::min(yMin, y));
		xMax = std::min(image->width  - 1, std::max(xMax, x));
		yMax = std::min(image->height - 1, std::max(yMax, y));
	}

	// Loop through every pixel in triangle's AABB.
	// Discard if barycentric coordinates are negative (point not in triangle)! xD
	for (int x = xMin; x <= xMax; x++)
	{
		for (int y = yMin; y <= yMax; y++)
		{
			Vector3 bc = Barycenter({ (float)x, (float)y, 0.0f },
				vertices[0], vertices[1], vertices[2]);

			if (bc.x < 0.0f || bc.y < 0.0f || bc.z < 0.0f)
				continue;

			// Looks cooler (pun not intended) with negative colours
			Vector3 p0 = positions[0];// * 0.5f + 0.5f;
			Vector3 p1 = positions[1];// * 0.5f + 0.5f;
			Vector3 p2 = positions[2];// * 0.5f + 0.5f;
			Vector3 p = p0 * bc.x + p1 * bc.y + p2 * bc.z;
			color = Float3ToColor(&p.x);

			//Vector2 uv0 = { 1.0f, 0.0f };
			//Vector2 uv1 = { 0.0f, 1.0f };
			//Vector2 uv2 = { 1.0f, 1.0f };
			//Vector2 uv = uv0 * bc.x + uv1 * bc.y + uv2 * bc.z;
			//Color tex = GetPixel(*image, uv.x * (float)image->width, uv.y * (float)image->height);
			
			// In OpenGL 0.0 --> near, 1.0 --> far.
			// Manually clearing depth to 1.0 every frame so I can do a < comparison.
			if (depth)
			{
				float z = 0.0f;
				z += vertices[0].z * bc.x;
				z += vertices[1].z * bc.y;
				z += vertices[2].z * bc.z;

				if (z < GetDepth(*image, x, y))
				{
					SetDepth(image, x, y, z);
					SetPixel(image, x, y, color);
				}
			}
		}
	}
}

// Culls back-facing triangles
inline void DrawFace(Image* image, Mesh mesh, size_t face)
{
	size_t vertex = face * 3;
	Vector3 v0 = mesh.positions[vertex + 0];
	Vector3 v1 = mesh.positions[vertex + 1];
	Vector3 v2 = mesh.positions[vertex + 2];
	Vector3 n = Normalize(Cross(v2 - v0, v1 - v0));
	Vector3 l{ 0.0f, 0.0f, -1.0f };

	float intensity = Dot(n, l);
	if (intensity > 0.0f)
	{
		Color color{ intensity * 255.0f,intensity * 255.0f, intensity * 255.0f, 255 };
		DrawTriangle(image, mesh, face, color);
	}
}

inline void DrawFaceWireframes(Image* image, Mesh mesh, size_t face)
{
	size_t vertex = face * 3;
	for (size_t i = 0; i < 3; i++)
	{
		Vector3 v0 = mesh.positions[vertex + i];
		Vector3 v1 = mesh.positions[vertex + ((i + 1) % 3)];
		v0.x = Remap(v0.x, -1.0f, 1.0f, 0, image->width - 1);
		v0.y = Remap(v0.y, -1.0f, 1.0f, 0, image->height - 1);
		v1.x = Remap(v1.x, -1.0f, 1.0f, 0, image->width - 1);
		v1.y = Remap(v1.y, -1.0f, 1.0f, 0, image->height - 1);

		int x0 = v0.x;
		int y0 = v0.y;
		int x1 = v1.x;
		int y1 = v1.y;
		DrawLine(image, x0, y0, x1, y1, WHITE);
	}
}