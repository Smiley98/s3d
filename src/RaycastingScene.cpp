#include "RaycastingScene.h"
#include "Mesh.h"
#include "Shader.h"
#include "Window.h"
#include "App.h"

constexpr Vector2 CENTER{ IMAGE_SIZE * 0.5f, IMAGE_SIZE * 0.5f };

struct Cell
{
	int row = -1;
	int col = -1;
};

using Cells = std::vector<Cell>;

// Gonna need to change this to step in grid space (for performance),
// but calculate screen-space edge every iteration. Will need to determine offset.
inline Cells DDA(int x0, int y0, int x1, int y1)
{
	// Make the horizontal component the largest (avoid divide by zero)!
	bool flipY = false;
	if (fabsf(x0 - x1) < fabsf(y0 - y1))
	{
		flipY = true;
		std::swap(x0, y0);
		std::swap(x1, y1);
	}
	
	// Make the line point left-to-right
	bool flipX = false;
	if (x0 > x1)
	{
		std::swap(x0, x1);
		std::swap(y0, y1);
		flipX = true;
	}

	size_t i = 0;
	Cells cells;
	cells.resize((x1 - x0) + 1);

	for (int x = x0; x <= x1; x++, i++)
	{
		// Calculate interpolation value
		float t = (x - x0) / (float)(x1 - x0);

		// Express y in terms of x by lerping!
		int y = y0 * (1.0f - t) + y1 * t;

		int px = flipY ? y : x;
		int py = flipY ? x : y;

		cells[i].row = py;
		cells[i].col = px;
	}

	if (flipX)
		std::reverse(cells.begin(), cells.end());

	return cells;
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
	LoadImage(mImage, IMAGE_SIZE, IMAGE_SIZE);
	mTexture = LoadTexture(mImage);

	mPosition = CENTER;
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
	mDirection = Normalize(mouse - mPosition);

	bool bounded = RectRect(8, 8, 496, 496, mouse.x - 3, mouse.y - 3, 6, 6);
	if (bounded)
		DrawCircle(mImage, mouse.x, mouse.y, 3, BLUE);
	DrawLine(mImage, mPosition.x, mPosition.y, mouse.x, mouse.y, BLUE);

	int mouseCol, mouseRow;
	ImageToTile(mouse.x, mouse.y, &mouseCol, &mouseRow);
	DrawTile(mImage, mouseCol, mouseRow, CYAN);

	int centerCol, centerRow;
	ImageToTile(mPosition.x, mPosition.y, &centerCol, &centerRow);

	if (IsKeyDown(KEY_SPACE))
	{
		//Vector2 poi;
		Cells cells;
		cells = DDA(centerCol, centerRow, mouseCol, mouseRow);

		for (size_t i = 0; i < cells.size(); i++)
		{
			Cell cell = cells[i];
			DrawTile(mImage, cell.col, cell.row, BLUE);
			int cx, cy;
			TileToImage(cell.col, cell.row, &cx, &cy);
			DrawCircle(mImage, cx, cy, 5, RED);

			Vector2 end = mPosition + mDirection * (i * TILE_SIZE);
			DrawCircle(mImage, end.x, end.y, 5, CYAN);
		}
		DrawTile(mImage, cells.back().col, cells.back().row, MAGENTA);
		//DrawCircle(mImage, poi.x, poi.y, 5, RED);
	}

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
