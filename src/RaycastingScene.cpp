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
RayHit RaycastingScene::DDATest(Vector2 position, Vector2 direction)
{
	int mapX, mapY;
	ImageToTile(position.x, position.y, &mapX, &mapY);

	Vector2 p = position / (float)TILE_SIZE;
	Vector2 s;
	Vector2 d;
	d.x = fabsf(1.0f / direction.x);
	d.y = fabsf(1.0f / direction.y);
	
	int stepX;
	int stepY;
	int side;
	
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

	TileType hit = AIR;
	while (hit == AIR)
	{
		if (s.x < s.y)
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

		hit = (TileType)mMap[mapY][mapX];
	}

	RayHit ray;
	ray.pos = position;
	ray.dir = direction;
	ray.t = side == 0 ? s.x - d.x : s.y - d.y;
	ray.poi = ray.pos + ray.dir * ray.t * TILE_SIZE;
	ray.type = hit;
	return ray;
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
	mDirection = Rotate(mDirection, -90.0 * DEG2RAD);
	//DDATest(Vector2{475.0f, 475.0f}, mDirection);

	LoadImage(mImage, IMAGE_SIZE, IMAGE_SIZE);
	mTexture = LoadTexture(mImage);
}

void RaycastingScene::OnUnload()
{
}

void RaycastingScene::OnUpdate(float dt)
{
	Color colors[]{ BLACK, RED, GREEN, BLUE, WHITE };
	for (size_t row = 0; row < MAP_SIZE; row++)
	{
		for (size_t col = 0; col < MAP_SIZE; col++)
		{
			int rx, ry;
			TileToImage(col, row, &rx, &ry);

			DrawTile(mImage, col, row, colors[mMap[row][col]]);
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

	float fov = 90.0f;
	//float view = tanf(fov * 0.5f * DEG2RAD);
	// 1.0 if 90

	mHits.clear();
	mHits.resize(mImage.width);
	for (size_t i = 0; i < mHits.size(); i++)
	{
		float rad = Remap(i, 0, mHits.size() - 1, -fov * 0.5f, fov * 0.5f) * DEG2RAD;
		Vector2 dir = Rotate(mDirection, rad);
		mHits[i] = DDATest(mPosition, dir);

		// Cosine weirdness when attempting correction with euclidean distance.
		// Article I'm following rotates the camera in addition to player so that's probably why t isn't fisheye-corrected.
		//float t = mHits[i].t;
		//float t2 = Distance(mHits[i].poi, mHits[i].pos);
		//float fisheye = fabsf(cosf(Angle(mDirection) - rad));

		mHits[i].t /= 20.0f;
		mHits[i].t = 1.0f - mHits[i].t;
	}

	for (int i = 0; i < mImage.width; i++)
	{
		float height = mHits[i].t * mImage.height;
		DrawLineY(mImage, i, 256 - height * 0.5f, 256 + height * 0.5f, colors[mHits[i].type]);
	}

	static bool drawDDA = false;
	if (IsKeyPressed(KEY_SPACE))
		drawDDA = !drawDDA;

	RayHit p = DDATest(mPosition, mDirection);
	DrawLine(mImage, mPosition.x, mPosition.y, p.poi.x, p.poi.y, RED);
	DrawCircle(mImage, p.poi.x, p.poi.y, 5, RED);

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