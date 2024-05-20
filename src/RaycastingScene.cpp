#include "RaycastingScene.h"
#include "Mesh.h"
#include "Shader.h"
#include "Window.h"
#include "App.h"

constexpr Vector2 CENTER{ IMAGE_SIZE * 0.5f, IMAGE_SIZE * 0.5f };

inline float TileOverlap(float position, float direction, float tileLength)
{
	float remainder = fmodf(position, tileLength);
	float edge = direction >= 0.0f ? position + tileLength - remainder : position - remainder;
	return edge - position;
}

inline Cells RaycastingScene::DDA(int x0, int y0, int x1, int y1)
{
	Cells cells;

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

		Cell cell;
		cell.row = y;
		cell.col = x;
		cells.push_back(cell);
	}

	return cells;
}

//Cells RaycastingScene::DDA(Vector2 position, Vector2 direction)
//{
//	Cells cells;
//
//	Vector2 poi = position;
//
//	TileType hit = AIR;
//	//while (hit == AIR)
//	//{
//	//	float tx = TileOverlap(poi.x)
//	//
//	//	//Cell cell;
//	//	//cell.row = y;
//	//	//cell.col = x;
//	//	//cells.push_back(cell);
//	//}
//
//	return cells;
//}

// The most important part is determining whether we will hit the wall first in x or y
inline Vector2 DDATest(Vector2 position, Vector2 direction)
{
	Vector2 poi;

	// Assuming direction is pointing right:
	//float px = position.x - fmodf(position.x, TILE_SIZE);
	//float py = position.y - fmodf(position.y, TILE_SIZE);

	float h = 1.0f / direction.x;
	float opp = sqrtf(h - 1);
	poi.x = position.x + TILE_SIZE * direction.x * h;
	poi.y = position.y + TILE_SIZE * direction.y * opp;
	return poi;
	//Vector2 p = position;
	//float t = 0.0f;
	//
	//float xOverlap = TileOverlap(position.x, direction.x, TILE_SIZE);
	//float yOverlap = TileOverlap(position.y, direction.y, TILE_SIZE);
	//
	//// We can 
	//
	//Vector2 d = direction;
	//d.x = fabsf(d.x) <= FLT_EPSILON ? FLT_EPSILON * Sign(d.x) : d.x;
	//d.y = fabsf(d.y) <= FLT_EPSILON ? FLT_EPSILON * Sign(d.y) : d.y;

	//bool stepX = xOverlap / d.x < yOverlap / d.y;
	//float t = stepX ? xOverlap / TILE_SIZE : yOverlap / TILE_SIZE;

	//if (stepX)
	//{
	//	bool right = d.x > 0.0f;
	//	t = xOverlap / TILE_SIZE;
	//}
	//else
	//{
	//	// Might have to invert y xD
	//	//bool down = d.y > 0.0f;
	//}

	//return p + Vector2{ xOverlap, yOverlap };
}

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

inline void ImageToTile(int x, int y, int* col, int* row)
{
	*col = x / TILE_SIZE;
	*row = y / TILE_SIZE;
}

inline void TileToImage(int col, int row, int* x, int* y)
{
	*x = col * TILE_SIZE;
	*y = row * TILE_SIZE;
}

inline void DrawTile(Image& image, int col, int row, Color color)
{
	int x, y;
	TileToImage(col, row, &x, &y);
	DrawRect(image, x, y, TILE_SIZE, TILE_SIZE, color);
}

void RaycastingScene::OnLoad()
{
	mPosition = CENTER;
	//mPosition.x += TILE_SIZE * 0.5f;
	//mPosition.y += TILE_SIZE * 0.5f;
	DDATest(mPosition, Rotate(mDirection, 130.0f * DEG2RAD));

	LoadImage(mImage, IMAGE_SIZE, IMAGE_SIZE);
	mTexture = LoadTexture(mImage);

	//Cells cells = DDA(2, 2, 3, 16);
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
			TileToImage(col, row, &rx, &ry);

			Color color = mMap[row][col] == WALL ? MAGENTA : BLACK;
			DrawTile(mImage, col, row, color);
		}
	}

	// DDA rendering test
	//Vector2 mouse = MousePosition();
	//mouse = ScreenToImage(mImage, mouse);
	//if (IsKeyDown(KEY_SPACE))
	//	DrawDDA(CENTER, mouse);

	float translationDelta = dt * mMoveSpeed;
	float rotationDelta = dt * mTurnSpeed * DEG2RAD;
	if (IsKeyDown(KEY_W))
	{
		mPosition += mDirection * translationDelta;
	}
	if (IsKeyDown(KEY_S))
	{
		mPosition -= mDirection * translationDelta;
	}
	if (IsKeyDown(KEY_A))
	{
		mDirection = Rotate(mDirection, -rotationDelta);
	}
	if (IsKeyDown(KEY_D))
	{
		mDirection = Rotate(mDirection, rotationDelta);
	}

	Vector2 start = mPosition;
	Vector2 end = mPosition + mDirection * 100.0f;
	DrawLine(mImage, start.x, start.y, end.x, end.y, RED);

	static bool drawDDA = false;
	if (IsKeyPressed(KEY_SPACE))
		drawDDA = !drawDDA;
	if (drawDDA)
		DrawDDA(start, end);

	Vector2 p = DDATest(mPosition, mDirection);
	DrawCircle(mImage, p.x, p.y, 5, MAGENTA);

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

void RaycastingScene::DrawDDA(Vector2 start, Vector2 end)
{
	int x0, y0, x1, y1;
	ImageToTile(start.x, start.y, &x0, &y0);
	ImageToTile(end.x, end.y, &x1, &y1);

	Cells cells = DDA(x0, y0, x1, y1);
	for (size_t i = 0; i < cells.size(); i++)
	{
		Cell cell = cells[i];
		int tx, ty;
		TileToImage(cell.col, cell.row, &tx, &ty);
		DrawTile(mImage, cell.col, cell.row, BLUE);
		DrawCircle(mImage, tx, ty, 5, RED);
	}
	if (!cells.empty())
		DrawTile(mImage, cells.back().col, cells.back().row, CYAN);
}