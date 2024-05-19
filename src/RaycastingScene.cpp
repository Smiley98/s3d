#include "RaycastingScene.h"
#include "Mesh.h"
#include "Shader.h"
#include "Window.h"
#include "App.h"

constexpr Vector2 CENTER{ IMAGE_SIZE * 0.5f, IMAGE_SIZE * 0.5f };

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

inline void ImageToTile(int ix, int iy, int& tx, int& ty)
{
	tx = ix / TILE_SIZE;
	ty = iy / TILE_SIZE;
}

inline void TileToImage(int tx, int ty, int& ix, int& iy)
{
	ix = tx * TILE_SIZE;
	iy = ty * TILE_SIZE;
}

void RaycastingScene::OnLoad()
{
	LoadImage(mImage, IMAGE_SIZE, IMAGE_SIZE);
	mTexture = LoadTexture(mImage);
}

void RaycastingScene::OnUnload()
{
}

void RaycastingScene::OnUpdate(float dt)
{
	for (size_t row = 0; row < MAP_SIZE; row++)
	{
		for (size_t col = 0; col < MAP_SIZE; col++)
		{
			int rx, ry;
			TileToImage(col, row, rx, ry);

			Color color = mMap[row][col] == WALL ? MAGENTA : BLACK;
			DrawRect(mImage, rx, ry, TILE_SIZE, TILE_SIZE, color);
		}
	}

	int x = 128;
	int y = 384;
	int r = 16;
	DrawCircle(mImage, x, y, r, RED);
	DrawCircleLines(mImage, x, y, r, MAGENTA);

	int w, h;
	w = h = 32;
	x = 384;
	DrawRect(mImage, x, y, w, h, RED);
	DrawRectLines(mImage, x, y, w, h, MAGENTA);
	
	float tt = TotalTime();
	float ncos = cosf(tt) * 0.5 + 0.5f;
	DrawCircle(mImage, ncos * IMAGE_SIZE, 256, 16, GREEN);
	//DrawCircle(mImage, ncos * IMAGE_SIZE, 500, 16, GREEN);
	// As long as we don't go out of bounds (<0 or > 511) we're fine.
	// Map has walls at edges, so we shouldn't be at risk!

	Vector2 mouse = ScreenToImage(mImage, MousePosition());
	bool bounded = RectRect(8, 8, 496, 496, mouse.x - 3, mouse.y - 3, 6, 6);
	if (bounded)
		DrawCircle(mImage, mouse.x, mouse.y, 3, BLUE);
	DrawLine(mImage, CENTER.x, CENTER.y, mouse.x, mouse.y, BLUE);

	// Flip since array [0, 0] = top-left but uv [0, 0] = bottom-left
	Flip(mImage);
}

void RaycastingScene::OnDraw()
{
	UpdateTexture(mTexture, mImage);

	BindTexture(mTexture);
	glUseProgram(gShaderFSQ);
	glUniform1i(glGetUniformLocation(gShaderFSQ, "u_tex"), 0);
	BindFsq();
	DrawFsq();

	glBindVertexArray(GL_NONE);
	glUseProgram(GL_NONE);
	UnbindTexture(mTexture);
}
