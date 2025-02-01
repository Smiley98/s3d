#include "DDAScene.h"
#include "Rasterization.h"
#include "ImageUtils.h"
#include "Window.h"
#include "Time.h"

void DDAScene::OnUpdate(float dt)
{
	for (int y = 0; y < CPU_IMAGE_SIZE; y++)
	{
		for (int x = 0; x < CPU_IMAGE_SIZE; x++)
		{
			Vector2 fragCoord{ x, y };
			Vector2 resolution{ CPU_IMAGE_SIZE, CPU_IMAGE_SIZE };
			Vector2 uv = fragCoord / resolution;
			Color color = Float2ToColor(&uv.x);
			SetPixel(&gImageCPU, x, y, color);
		}
	}
	Flip(&gImageCPU);

	Vector2 mouse = GetMousePosition();
	mouse = ScreenToImage(gImageCPU, mouse);
	int r = 3;
	int x0 = CPU_IMAGE_SIZE * 0.5;
	int y0 = CPU_IMAGE_SIZE * 0.5;

	// Make sure mouse doesn't go off-screen (prevent image array out of bounds)
	bool bounded = RectRect(8, 8, 496, 496, mouse.x - r, mouse.y - r, r * 2, r * 2);
	if (bounded)
	{
		DrawLine(&gImageCPU, x0, y0, mouse.x, mouse.y, RED);
		DrawCircle(&gImageCPU, x0, y0, r, RED);
		DrawCircle(&gImageCPU, mouse.x, mouse.y, r, RED);
	}

	// Animate a circle in a circle xD xD xD
	float tt = TotalTime();
	Matrix m = Translate(100.0f, 100.0f, 0.0f) * RotateZ(tt * 100.0f * DEG2RAD) * Translate(256.0f, 256.0f, 0.0f);
	Vector3 v = m * V3_ZERO;
	DrawCircle(&gImageCPU, v.x, v.y, 10, CYAN);
	DrawLine(&gImageCPU, x0, y0, v.x, v.y, CYAN);

	Flip(&gImageCPU);
}
