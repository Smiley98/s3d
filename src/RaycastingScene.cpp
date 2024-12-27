#include "RaycastingScene.h"
#include "Window.h"
#include "Render.h"
#include "Rasterization.h"

constexpr size_t MAP_SIZE = 16;
constexpr size_t IMAGE_SIZE = 512;
constexpr size_t TILE_SIZE = IMAGE_SIZE / MAP_SIZE;
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

inline void DrawTile(Image* image, int col, int row, Color color)
{
	int x, y;
	TileToImage(col, row, &x, &y);
	DrawRect(image, x, y, TILE_SIZE, TILE_SIZE, color);
}

// This function doesn't need to be a class member. Only member is mMap which can easily be passed in!
RaycastingScene::RayHit RaycastingScene::Raycast(Vector2 position, Vector2 direction)
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

	size_t hit = 0;
	while (hit == 0)
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

		hit = mMap[mapY][mapX];
	}

	RayHit ray;
	ray.t = side == 0 ? s.x - d.x : s.y - d.y;
	ray.i = hit;
	return ray;
}

void RaycastingScene::OnLoad()
{
	CreateImageFromMemory(&mImage, IMAGE_SIZE, IMAGE_SIZE);
	CreateTextureFromMemory(&mTexture, IMAGE_SIZE, IMAGE_SIZE, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, GL_NEAREST);

	mHits.resize(mImage.width);
	mPosition = CENTER;
}

void RaycastingScene::OnUnload()
{
	DestroyTexture(&mTexture);
	DestroyImage(&mImage);
}

void RaycastingScene::OnUpdate(float dt)
{
	// See mMap declaration (values of mMap map 1:1 to color indices).
	Color colors[]{ BLACK, RED, GREEN, BLUE, WHITE };
	for (size_t row = 0; row < MAP_SIZE; row++)
	{
		for (size_t col = 0; col < MAP_SIZE; col++)
		{
			DrawTile(&mImage, col, row, colors[mMap[row][col]]);
		}
	}

	for (size_t row = 0; row < IMAGE_SIZE; row += TILE_SIZE)
		SetRow(&mImage, row, GRAY);

	for (size_t col = 0; col < IMAGE_SIZE; col += TILE_SIZE)
		SetCol(&mImage, col, GRAY);

	// TODO -- Add circle-plane for sides & circle-rectangle for tiles,
	// but not now so I don't accidentally leak assignment & lab solutions to students!
	float translationDelta = dt * mMoveSpeed;
	float rotationDelta = dt * mTurnSpeed * DEG2RAD;
	if (IsKeyDown(KEY_W))
		mPosition += mDirection * translationDelta;
	if (IsKeyDown(KEY_S))
		mPosition -= mDirection * translationDelta;
	if (IsKeyDown(KEY_A))
		mDirection = Rotate(mDirection, -rotationDelta);
	if (IsKeyDown(KEY_D))
		mDirection = Rotate(mDirection, rotationDelta);

	for (size_t i = 0; i < mHits.size(); i++)
	{
		// Ray angle is relative to viewer (always [-half fov, half fov])
		float rayAngle = Remap(i, 0, mHits.size() - 1, -PI * 0.25f, PI * 0.25f);
		float fisheye = cosf(rayAngle);
		mHits[i] = Raycast(mPosition, Rotate(mDirection, rayAngle));
		mHits[i].t *= fisheye;
	}

	for (int i = 0; i < mImage.width; i++)
	{
		float height = fminf(100.0f / mHits[i].t, mImage.height - 1);
		DrawLineY(&mImage, i, 256 - height * 0.5f, 256 + height * 0.5f, colors[mHits[i].i]);
	}

	RayHit hit = Raycast(mPosition, mDirection);
	Vector2 poi = mPosition + mDirection * hit.t * TILE_SIZE;
	DrawLine(&mImage, mPosition.x, mPosition.y, poi.x, poi.y, RED);
	DrawCircle(&mImage, poi.x, poi.y, 5, RED);

	// Flip since array [0, 0] = top-left but uv [0, 0] = bottom-left
	Flip(&mImage);
}

void RaycastingScene::OnDraw()
{
	UpdateTexture(mTexture, (unsigned char*)mImage.pixels.data());
	DrawFsqTexture(mTexture);
}
