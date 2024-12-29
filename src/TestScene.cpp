#include "TestScene.h"
#include "Time.h"
#include "Render.h"
#include "ImageUtils.h"

static Image fImage;

inline float Fract(float f)
{
	return f - floorf(f);
}

void TestScene::OnLoad()
{
	CreateImageDefault(&fImage);
}

void TestScene::OnUnload()
{
	DestroyImage(&fImage);
}

void TestScene::OnUpdate(float dt)
{
	// Palette parameters
	Vector3 a{ 0.5f, 0.5f, 0.5f };
	Vector3 b{ 0.5f, 0.5f, 0.5f };
	Vector3 c{ 1.0f, 1.0f, 1.0f };
	Vector3 d{ 0.263f, 0.416f, 0.557f };
	
	float tt = TotalTime();
	for (int y = 0; y < fImage.height; y++)
	{
		for (int x = 0; x < fImage.width; x++)
		{
			Vector2 fragCoord{ x, y };
			Vector2 resolution{ fImage.width, fImage.height };
			Vector2 uv = fragCoord / resolution;
			uv = uv * 2.0f - 1.0f;
			uv.x *= SCREEN_ASPECT;

			Vector2 uv0 = uv;
			Vector3 finalColor = V3_ZERO;

			for (float k = 0.0f; k < 4.0f; k++)
			{
				uv.x = Fract(uv.x * 1.5f) - 0.5f;
				uv.y = Fract(uv.y * 1.5f) - 0.5f;

				float t = Length(uv0) + k * 0.4f + tt * 0.4f;
				Vector3 col = Palette(a, b, c, d, t);
				float d = Length(uv);
				d = sinf(d * 8.0f + tt) / 8.0f;
				d = fabsf(d);
				d = powf(0.01f / d, 1.0f);

				finalColor += col * d;
			}

			Color color = Float3ToColor(&finalColor.x);
			SetPixel(&fImage, x, y, color);
		}
	}
}

void TestScene::OnDraw()
{
	Present(&fImage);
}
