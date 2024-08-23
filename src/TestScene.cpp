#include "TestScene.h"
#include "Shader.h"
#include "Mesh.h"
#include "App.h"
#include "ImageUtils.h"
#include "Time.h"

float Fract(float f)
{
	return f - floorf(f);
}

void TestScene::OnLoad()
{
	LoadImage(&mImage, 512, 512);
	LoadTexture(&mTexture, 512, 512);
}

void TestScene::OnUnload()
{
	UnloadTexture(&mTexture);
	UnloadImage(&mImage);
}

void TestScene::OnUpdate(float dt)
{
	// Palette parameters
	Vector3 a{ 0.5f, 0.5f, 0.5f };
	Vector3 b{ 0.5f, 0.5f, 0.5f };
	Vector3 c{ 1.0f, 1.0f, 1.0f };
	Vector3 d{ 0.263f, 0.416f, 0.557f };
	
	float tt = TotalTime();
	for (int y = 0; y < mImage.height; y++)
	{
		for (int x = 0; x < mImage.width; x++)
		{
			Vector2 fragCoord{ x, y };
			Vector2 resolution{ mImage.width, mImage.height };
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
			SetPixel(&mImage, x, y, color);
		}
	}
}

void TestScene::OnDraw()
{
	UpdateTexture(mTexture, mImage);

	BindTexture(mTexture);
	BindShader(&gShaderFSQ);
	SendInt("u_tex", 0);
	BindFsq();
	DrawFsq();
	UnbindShader();
	UnbindTexture(mTexture);
}
