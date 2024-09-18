#include "DDAScene.h"
#include "Window.h"
#include "Time.h"
#include "Mesh.h"
#include "Shader.h"
#include "Render.h"
#include "Rasterization.h"
#include "ImageUtils.h"

constexpr size_t IMAGE_SIZE = 512;

void DDAScene::OnLoad()
{
	LoadImage(&mImage, IMAGE_SIZE, IMAGE_SIZE);
	CreateTexture(&mTexture, IMAGE_SIZE, IMAGE_SIZE);
}

void DDAScene::OnUnload()
{
	DestroyTexture(&mTexture);
	UnloadImage(&mImage);
}

void DDAScene::OnUpdate(float dt)
{
	for (int y = 0; y < mImage.height; y++)
	{
		for (int x = 0; x < mImage.width; x++)
		{
			Vector2 fragCoord{ x, y };
			Vector2 resolution{ mImage.width, mImage.height };
			Vector2 uv = fragCoord / resolution;
			Color color = Float2ToColor(&uv.x);
			SetPixel(&mImage, x, y, color);
		}
	}
	Flip(&mImage);

	Vector2 mouse = MousePosition();
	mouse = ScreenToImage(mImage, mouse);
	int r = 3;
	int x0 = mImage.width * 0.5;
	int y0 = mImage.height * 0.5;

	// Make sure mouse doesn't go off-screen (prevent image array out of bounds)
	bool bounded = RectRect(8, 8, 496, 496, mouse.x - r, mouse.y - r, r * 2, r * 2);
	if (bounded)
	{
		DrawLine(&mImage, x0, y0, mouse.x, mouse.y, RED);
		DrawCircle(&mImage, x0, y0, r, RED);
		DrawCircle(&mImage, mouse.x, mouse.y, r, RED);
	}

	// Animate a circle in a circle xD xD xD
	float tt = TotalTime();
	Matrix m = Translate(100.0f, 100.0f, 0.0f) * RotateZ(tt * 100.0f * DEG2RAD) * Translate(256.0f, 256.0f, 0.0f);
	Vector3 v = m * V3_ZERO;
	DrawCircle(&mImage, v.x, v.y, 10, CYAN);
	DrawLine(&mImage, x0, y0, v.x, v.y, CYAN);

	Flip(&mImage);
}

void DDAScene::OnDraw()
{
	UpdateTexture(mTexture, mImage);
	DrawFsqTexture(mTexture);
}
