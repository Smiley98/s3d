#include "RaycastingScene.h"
#include "Mesh.h"
#include "Shader.h"
#include "Window.h"
#include "App.h"

constexpr Vector2 CENTER{ IMAGE_SIZE * 0.5f, IMAGE_SIZE * 0.5f };

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

inline float TileOverlap(float position, float direction, float tileLength)
{
	float remainder = fmodf(position, tileLength);
	float edge = direction >= 0.0f ? position + tileLength - remainder : position - remainder;
	return edge - position;
}

inline float TilePercentage(float position, float direction)
{
	float r = fmodf(position, TILE_SIZE);
	float t = r / TILE_SIZE;
	return direction >= 0.0f ? t : 1.0f - t;
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

using Points = std::vector<Vector2>;
Points pois;
inline Vector2 RaycastingScene::DDATest(Vector2 position, Vector2 direction)
{
	int mapX, mapY;
	ImageToTile(position.x, position.y, &mapX, &mapY);

	Vector2 p = position / (float)TILE_SIZE;
	Vector2 d = Vector2One() / direction;
	Vector2 s;
	
	int stepX;
	int stepY;
	int side;
	
	// Step 1 -- Calculate s; how far we need to move in direction to intersect an axis.
	// 
	// Example: direction = [0.5, 0.86] (60 degrees), p = 8.5 (middle of tile in middle of grid).
	// 1. Calculate t:
	// t = mapX + 1 - pX
	// t = 8 + 1 - 8.5
	// t = 0.5 (50% makes sense cause player is half way across its tile).
	// 
	// 2. Calculate s:
	// d = inverse direction --> moving d magnitude in direction moves us 1 unit (d.x * direction.x = 1)
	// d.x = 1.0 / 0.5 = 2.0
	// s = t * d --> "if we're 50% across the tile, and d moves us fully across, we must move d * 0.5!"
	// s.x = 0.5 * 2 = 1
	// 
	// Proof -- must move 1 in direction.x to move player to right of tile:
	// right = p.x + s.x * direction.x
	// right = 8.5 + 1.0 * 0.5
	// right = 9.0
	if (direction.x < 0.0f)
	{
		stepX = -1;
		s.x = (p.x - mapX) * d.x;
	}
	else
	{
		stepX = 1;
		s.x = (mapX + 1.0f - p.x) * d.x;
	}
	if (direction.y < 0.0f)
	{
		stepY = -1;
		s.y = (p.y - mapY) * d.y;
	}
	else
	{
		stepY = 1;
		s.y = (mapY + 1.0f - p.y) * d.y;
	}

	pois.clear();

	// Step 2: Now that we've calculated how far to move in direction to intersect x & y axis,
	// we must march until we've hit a wall!
	// Do so by incrementing the nearest axis by d to move 1 unit.
	TileType hit = AIR;
	while (hit == AIR)
	{
		if (fabsf(s.x) < fabsf(s.y))
		{
			s.x += d.x;
			mapX += stepX;
			side = 0;
		}
		else
		{
			s.y += d.y;
			mapY += stepY;
			side = 1;
		}

		float t = fabsf(side == 0 ? s.x - d.x : s.y - d.y);
		Vector2 poi = position + direction * t * TILE_SIZE;
		pois.push_back(poi);
		hit = (TileType)mMap[mapY][mapX];
	}

	float dist = side == 0 ? s.x - d.x : s.y - d.y;
	return position + direction * fabsf(dist) * TILE_SIZE;
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

inline void DrawTile(Image& image, int col, int row, Color color)
{
	int x, y;
	TileToImage(col, row, &x, &y);
	DrawRect(image, x, y, TILE_SIZE, TILE_SIZE, color);
}

void RaycastingScene::OnLoad()
{
	mPosition = CENTER;
	mPosition.x += TILE_SIZE * 0.5f;
	mPosition.y += TILE_SIZE * 0.5f;
	mDirection = Rotate(mDirection, 60.0f * DEG2RAD);
	//DDATest(mPosition, mDirection);

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
			TileToImage(col, row, &rx, &ry);

			Color color = mMap[row][col] == WALL ? MAGENTA : BLACK;
			DrawTile(mImage, col, row, color);
		}
	}

	for (size_t row = 0; row < IMAGE_SIZE; row += TILE_SIZE)
	{
		SetRow(mImage, row, GRAY);
	}

	for (size_t col = 0; col < IMAGE_SIZE; col += TILE_SIZE)
	{
		SetCol(mImage, col, GRAY);
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
	//mDirection = Normalize(MousePosition() - mPosition);

	static bool drawDDA = false;
	if (IsKeyPressed(KEY_SPACE))
		drawDDA = !drawDDA;

	Vector2 p = DDATest(mPosition, mDirection);
	for (Vector2 point : pois)
		DrawCircle(mImage, point.x, point.y, 2, RED);
	DrawLine(mImage, mPosition.x, mPosition.y, p.x, p.y, RED);
	DrawCircle(mImage, p.x, p.y, 5, RED);

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