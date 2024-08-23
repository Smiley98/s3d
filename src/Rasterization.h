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

inline void DrawRectLines(Image* image, int x, int y, int w, int h, Color color)
{
	DrawLineX(image, y + 0, x, x + w, color);
	DrawLineX(image, y + h, x, x + w, color);
	DrawLineY(image, x + 0, y, y + h, color);
	DrawLineY(image, x + w, y, y + h, color);
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

inline void DrawFaceWireframes(Image* image, Vector3* positions, size_t face, Color color = WHITE)
{
	const int xMin = 0;
	const int yMin = 0;
	const int xMax = image->width - 1;
	const int yMax = image->height - 1;
	size_t vertex = face * 3;
	for (size_t i = 0; i < 3; i++)
	{
		Vector3 v0 = positions[vertex + i];
		Vector3 v1 = positions[vertex + ((i + 1) % 3)];

		v0.x = Remap(v0.x, -1.0f, 1.0f, xMin, xMax);
		v0.y = Remap(v0.y, -1.0f, 1.0f, yMin, yMax);
		v1.x = Remap(v1.x, -1.0f, 1.0f, xMin, xMax);
		v1.y = Remap(v1.y, -1.0f, 1.0f, yMin, yMax);

		int x0 = v0.x;
		int y0 = v0.y;
		int x1 = v1.x;
		int y1 = v1.y;

		// Better if I did a line-rectangle intersection with the screen
		//if (x0 < xMin || x0 > xMax || x1 < xMin || x1 > xMax ||
		//	y0 < yMin || y0 > yMax || y1 < yMin || y1 > yMax)
		//	continue;

		DrawLine(image, x0, y0, x1, y1, color);
	}
}

struct UniformData
{
	Matrix mvp;
	Matrix world;

	Vector3 cameraPosition;
	Vector3 lightPosition;
	Vector3 lightColor;

	float ambient;
	float diffuse;
	float specular;
};

inline Vector3 Phong(Vector3 position, Vector3 normal, Vector3 camera, Vector3 light,
	Vector3 color, float ambient, float diffuse, float specular)
{
	Vector3 N = normal;
	Vector3 L = Normalize(light - position);
	Vector3 V = Normalize(camera - position);
	Vector3 R = Reflect(L * -1.0f, N);

	float dotNL = fmaxf(Dot(N, L), 0.0f);
	float dotVR = fmaxf(Dot(V, R), 0.0f);

	Vector3 phong = V3_ZERO;
	phong += color * ambient;
	phong += color * diffuse * dotNL;
	phong += color * powf(dotVR, specular);
	return phong;
}

inline void DrawMesh(Image* image, Mesh mesh, UniformData uniform)
{
	// Vertex input begin
	Vector3* vertices = new Vector3[mesh.vertexCount];
	Vector3* ndcs = new Vector3[mesh.vertexCount];
	Vector3* positions = new Vector3[mesh.vertexCount];
	Vector3* normals = new Vector3[mesh.vertexCount];
	Vector2* tcoords = new Vector2[mesh.vertexCount];
	// Vertex input end

	Matrix normalMatrix = NormalMatrix(uniform.world);

	// Vertex shader begin
	for (size_t i = 0; i < mesh.vertexCount; i++)
	{
		Vector3 ndc = Clip(mesh.positions[i], uniform.mvp);
		Vector3 screen = ndc;
		screen.x = Remap(screen.x, -1.0f, 1.0f, 0.0f, image->width - 1.0f);
		screen.y = Remap(screen.y, -1.0f, 1.0f, 0.0f, image->height - 1.0f);

		vertices[i] = screen;
		ndcs[i] = ndc;
		positions[i] = uniform.world * mesh.positions[i];
		normals[i] = Normalize(normalMatrix * mesh.normals[i]);
		tcoords[i] = mesh.tcoords[i];
	}
	// Vertex shader end

	// Triangle AABBs
	Rect* rects = new Rect[mesh.faceCount];
	for (size_t face = 0; face < mesh.faceCount; face++)
	{
		// Ensure min & max get overwritten
		int xMin = image->width - 1;
		int yMin = image->height - 1;
		int xMax = 0;
		int yMax = 0;

		// Determine min & max of face, ensure its on-screen
		const size_t vertex = face * 3;
		for (size_t i = 0; i < 3; i++)
		{
			int x = vertices[vertex + i].x;
			int y = vertices[vertex + i].y;
			xMin = std::max(0, std::min(xMin, x));
			yMin = std::max(0, std::min(yMin, y));
			xMax = std::min(image->width - 1, std::max(xMax, x));
			yMax = std::min(image->height - 1, std::max(yMax, y));
		}

		// Face culling
		Vector3 v0 = ndcs[vertex + 0];
		Vector3 v1 = ndcs[vertex + 1];
		Vector3 v2 = ndcs[vertex + 2];
		Vector3 n = Normalize(Cross(v1 - v0, v2 - v1));
		bool front = Dot(n, V3_FORWARD) > 0.0f;
		if (front)
		{
			rects[face].xMin = xMin;
			rects[face].xMax = xMax;
			rects[face].yMin = yMin;
			rects[face].yMax = yMax;
		}
		else
		{
			rects[face].xMin = 0;
			rects[face].xMax = -1;
			rects[face].yMin = 0;
			rects[face].yMax = -1;
		}
	}

	for (size_t face = 0; face < mesh.faceCount; face++)
	{
		for (int x = rects[face].xMin; x <= rects[face].xMax; x++)
		{
			for (int y = rects[face].yMin; y <= rects[face].yMax; y++)
			{
				// Rasterizer begin
				size_t vertex = face * 3;
				Vector3 v0 = vertices[vertex + 0];
				Vector3 v1 = vertices[vertex + 1];
				Vector3 v2 = vertices[vertex + 2];

				Vector3 bc = Barycenter({ (float)x, (float)y, 0.0f }, v0, v1, v2);
				bool low = bc.x < 0.0f || bc.y < 0.0f || bc.z < 0.0f;
				bool high = bc.x > 1.0f || bc.y > 1.0f || bc.z > 1.0f;
				bool nan = _isnanf(bc.x) || _isnanf(bc.y) || _isnanf(bc.z);
				// nan only seems to affect uvs but not positions or normals...

				if (low || high || nan)
					continue;

				float depth = v0.z * bc.x + v1.z * bc.y + v2.z * bc.z;
				if (depth > GetDepth(*image, x, y))
					continue;
				SetDepth(image, x, y, depth);
				// Rasterizer end

				// Fragment shader begin
				Vector3 p0 = positions[vertex + 0];
				Vector3 p1 = positions[vertex + 1];
				Vector3 p2 = positions[vertex + 2];
				Vector3 p = Terp(p0, p1, p2, bc);

				Vector3 n0 = normals[vertex + 0];
				Vector3 n1 = normals[vertex + 1];
				Vector3 n2 = normals[vertex + 2];
				Vector3 n = Terp(n0, n1, n2, bc);

				Vector2 uv0 = tcoords[vertex + 0];
				Vector2 uv1 = tcoords[vertex + 1];
				Vector2 uv2 = tcoords[vertex + 2];
				Vector2 uv = Terp(uv0, uv1, uv2, bc);

				float tw = gImageDiffuse.width;
				float th = gImageDiffuse.height;
				Color textureColor = GetPixel(gImageDiffuse, uv.x * tw, uv.y * th);

				Vector3 baseColor = Phong(p, n, uniform.cameraPosition, uniform.lightPosition,
					uniform.lightColor, uniform.ambient, uniform.diffuse, uniform.specular);

				Vector3 pixelColor{ textureColor.r, textureColor.g, textureColor.b };
				pixelColor /= 255.0f;
				pixelColor *= baseColor;

				Color color = Float3ToColor(&pixelColor.x);
				SetPixel(image, x, y, color);
				// Fragment shader end
			}
		}
	}

	delete[] rects;
	delete[] tcoords;
	delete[] normals;
	delete[] positions;
	delete[] ndcs;
	delete[] vertices;
}
