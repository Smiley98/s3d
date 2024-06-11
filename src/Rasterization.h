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
inline void DrawTriangle(Image* image, Mesh mesh, size_t face, Vector3 tint = V3_ONE, bool depth = true)
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
	for (int x = xMin; x <= xMax; x++)
	{
		for (int y = yMin; y <= yMax; y++)
		{
			Vector3 bc = Barycenter({ (float)x, (float)y, 0.0f },
				vertices[0], vertices[1], vertices[2]);

			// Discard if barycentric coordinates are negative (point not in triangle)! xD
			if (bc.x < 0.0f || bc.y < 0.0f || bc.z < 0.0f)
				continue;
			Color color = BLACK;

			Vector3 p0 = positions[0];
			Vector3 p1 = positions[1];
			Vector3 p2 = positions[2];
			Vector3 p = p0 * bc.x + p1 * bc.y + p2 * bc.z;
			color = Float3ToColor(&p.x);

			Vector3 n0 = normals[0];
			Vector3 n1 = normals[1];
			Vector3 n2 = normals[2];
			Vector3 n = n0 * bc.x + n1 * bc.y + n2 * bc.z;
			color = Float3ToColor(&n.x);

			Vector2 uv0 = tcoords[0];
			Vector2 uv1 = tcoords[1];
			Vector2 uv2 = tcoords[2];
			Vector2 uv = uv0 * bc.x + uv1 * bc.y + uv2 * bc.z;

			float tw = gImageDiffuse.width;
			float th = gImageDiffuse.height;
			color = GetPixel(gImageDiffuse, uv.x * tw, uv.y * th);
			Vector3 c{ color.r, color.g, color.b };
			c /= 255.0f;
			c *= tint;
			color = Float3ToColor(&c.x);
			
			// Clip OpenGL: -1 = near, +1 = far.
			// Clip DirectX: 0 = near,  1 = far.
			// Seems things only work correctly with z = 0, clear depth = -1, & cmp GREATER.
			// Probably gonna understand this better once we convert to clip-space.
			// (Dunno if tutorial follows DX or GL's clip-space).
			if (depth)
			{
				float z = 0.0f;
				z += vertices[0].z * bc.x;
				z += vertices[1].z * bc.y;
				z += vertices[2].z * bc.z;

				if (z > GetDepth(*image, x, y))
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
	// Assumes -z is forward.

	float intensity = Dot(n, l);
	if (intensity > 0.0f)
		DrawTriangle(image, mesh, face, V3_ONE * intensity);
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

inline void DrawMesh(Image* image, Mesh mesh, Matrix mvp, Matrix world/*, Matrix normal*/)
{
	Vector3* vertices = new Vector3[mesh.vertexCount];	// clip-space
	Vector3* positions = new Vector3[mesh.vertexCount];	// world-space
	Vector3* normals = new Vector3[mesh.vertexCount];	// object-space (soon to be world-space)
	Vector2* tcoords = new Vector2[mesh.vertexCount];	// object-space
	Rect* rects = new Rect[mesh.faceCount];				// Triangle AABBs

	// Vertex pre-processing (space-transformations)
	for (size_t vertex = 0; vertex < mesh.vertexCount; vertex++)
	{
		Vector4 clip;
		clip.x = mesh.positions[vertex].x;
		clip.y = mesh.positions[vertex].y;
		clip.z = mesh.positions[vertex].z;
		clip.w = 1.0f;

		clip = mvp * clip;
		clip /= clip.w;

		Vector3 screen;
		screen.x = Remap(clip.x, -1.0f, 1.0f, 0.0f, (float)image->width - 1.0f);
		screen.y = Remap(clip.y, -1.0f, 1.0f, 0.0f, (float)image->height - 1.0f);
		screen.z = clip.z;
		vertices[vertex] = screen;

		positions[vertex] = world * mesh.positions[vertex];
		normals[vertex] = mesh.normals[vertex];
		tcoords[vertex] = mesh.tcoords[vertex];
	}

	// Triangle AABBs:
	for (size_t face = 0; face < mesh.faceCount; face++)
	{
		int xMin = image->width - 1;
		int yMin = image->height - 1;
		int xMax = 0;
		int yMax = 0;
		size_t vertex = face * 3;
		for (size_t i = 0; i < 3; i++)
		{	
			int x = vertices[vertex + i].x;
			int y = vertices[vertex + i].y;
			xMin = std::max(0, std::min(xMin, x));
			yMin = std::max(0, std::min(yMin, y));
			xMax = std::min(image->width - 1, std::max(xMax, x));
			yMax = std::min(image->height - 1, std::max(yMax, y));
		}
		rects[face].xMin = xMin;
		rects[face].xMax = xMax;
		rects[face].yMin = yMin;
		rects[face].yMax = yMax;
	}

	// Back-face culling:
	bool* frontFacing = new bool[mesh.faceCount];
	for (size_t face = 0; face < mesh.faceCount; face++)
	{
		size_t vertex = face * 3;
		Vector3 p0 = positions[vertex + 0];
		Vector3 p1 = positions[vertex + 1];
		Vector3 p2 = positions[vertex + 2];
		Vector3 n = Normalize(Cross(p2 - p0, p1 - p0));
		Vector3 l{ 0.0f, 0.0f, -1.0f };
		float intensity = Dot(n, l);
		frontFacing[face] = intensity > 0.0;
	}

	// Draw triangles:
	for (size_t face = 0; face < mesh.faceCount; face++)
	{
		// Discard if back-facing
		if (!frontFacing[face])
			continue;

		size_t vertex = face * 3;
		Vector3 v0 = vertices[vertex + 0];
		Vector3 v1 = vertices[vertex + 1];
		Vector3 v2 = vertices[vertex + 2];

		Vector3 n0 = normals[vertex + 0];
		Vector3 n1 = normals[vertex + 1];
		Vector3 n2 = normals[vertex + 2];

		Vector2 uv0 = tcoords[vertex + 0];
		Vector2 uv1 = tcoords[vertex + 1];
		Vector2 uv2 = tcoords[vertex + 2];

		for (int x = rects[face].xMin; x <= rects[face].xMax; x++)
		{
			for (int y = rects[face].yMin; y <= rects[face].yMax; y++)
			{
				Vector3 bc = Barycenter({ (float)x, (float)y, 0.0f },v0, v1, v2);

				// Discard if pixel not in triangle
				if (bc.x < 0.0f || bc.y < 0.0f || bc.z < 0.0f)
					continue;

				// Discard if depth test fails
				float z = 0.0f;
				z += v0.z * bc.x;
				z += v1.z * bc.y;
				z += v2.z * bc.z;
				if (z > GetDepth(*image, x, y))
					continue;
				// Note that proj allows us to use depth intuitively!
				// [-1 <= NEAR < z < FAR <= 1] (or maybe its 0 = near, test this).

				Color color = BLACK;
				//Vector3 v = v0 * bc.x + v1 * bc.y + v2 * bc.z;
				//Vector3 n = n0 * bc.x + n1 * bc.y + n2 * bc.z;
				Vector2 uv = uv0 * bc.x + uv1 * bc.y + uv2 * bc.z;
				float tw = gImageDiffuse.width;
				float th = gImageDiffuse.height;
				color = GetPixel(gImageDiffuse, uv.x * tw, uv.y * th);

				// *Insert point-in-screen test here*
				SetPixel(image, x, y, color);
				SetDepth(image, x, y, z);

				//color = Float3ToColor(&n.x);
				//color = Float3ToColor(&v.x);

				//Vector3 c{ color.r, color.g, color.b };
				//c /= 255.0f;
				//c *= tint;
				//color = Float3ToColor(&c.x);
			}
		}
	}

	delete[] frontFacing;
	delete[] rects;
	delete[] tcoords;
	delete[] normals;
	delete[] positions;
	delete[] vertices;
}

// Not going to handle vertices outside of clip-space;
// Just render a mesh with all pixels on screen for my sanity's sake!

// TODO -- test with world-space normals instead.
// This is fine for now since we're not computing normals so ~same efficiency!

// *Insert screen-culling & viewport transform here*
// Probably some sophisticated algorithm to reform triangles to prevent overdraw...
// Can probably just do if > 0 && < screen to preven out of bounds.
