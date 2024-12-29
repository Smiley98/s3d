#include "DDAScene.h"
#include "Window.h"
#include "Time.h"
#include "Render.h"
#include "Rasterization.h"
#include "ImageUtils.h"

static Image fImage;

void DDAScene::OnLoad()
{
	CreateImageDefault(&fImage);
}

void DDAScene::OnUnload()
{
	DestroyImage(&fImage);
}

void DDAScene::OnUpdate(float dt)
{
	for (int y = 0; y < fImage.height; y++)
	{
		for (int x = 0; x < fImage.width; x++)
		{
			Vector2 fragCoord{ x, y };
			Vector2 resolution{ fImage.width, fImage.height };
			Vector2 uv = fragCoord / resolution;
			Color color = Float2ToColor(&uv.x);
			SetPixel(&fImage, x, y, color);
		}
	}
	Flip(&fImage);

	Vector2 mouse = GetMousePosition();
	mouse = ScreenToImage(fImage, mouse);
	int r = 3;
	int x0 = fImage.width * 0.5;
	int y0 = fImage.height * 0.5;

	// Make sure mouse doesn't go off-screen (prevent image array out of bounds)
	bool bounded = RectRect(8, 8, 496, 496, mouse.x - r, mouse.y - r, r * 2, r * 2);
	if (bounded)
	{
		DrawLine(&fImage, x0, y0, mouse.x, mouse.y, RED);
		DrawCircle(&fImage, x0, y0, r, RED);
		DrawCircle(&fImage, mouse.x, mouse.y, r, RED);
	}

	// Animate a circle in a circle xD xD xD
	float tt = TotalTime();
	Matrix m = Translate(100.0f, 100.0f, 0.0f) * RotateZ(tt * 100.0f * DEG2RAD) * Translate(256.0f, 256.0f, 0.0f);
	Vector3 v = m * V3_ZERO;
	DrawCircle(&fImage, v.x, v.y, 10, CYAN);
	DrawLine(&fImage, x0, y0, v.x, v.y, CYAN);

	Flip(&fImage);
}

void DDAScene::OnDraw()
{
	Present(&fImage);
}
