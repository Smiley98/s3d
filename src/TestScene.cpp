#include "TestScene.h"
#include "Shader.h"
#include "Mesh.h"
#include "App.h"

float Fract(float f)
{
	return f - floorf(f);
}

Color3 Palette(float t) {
	Color3 a { 0.5f, 0.5f, 0.5f };
	Color3 b { 0.5f, 0.5f, 0.5f };
	Color3 c { 1.0f, 1.0f, 1.0f };
	Color3 d{ 0.263f, 0.416f, 0.557f };

	Color3 cd = (c * t + d) * 6.28318f;
	cd.r = cosf(cd.r);
	cd.g = cosf(cd.g);
	cd.b = cosf(cd.b);

	return a + b * cd;
}

void TestScene::OnLoad()
{
	LoadImage(mImage, 512, 512);
	LoadTexture(&mTexture, 512, 512);
}

void TestScene::OnUnload()
{
	UnloadTexture(&mTexture);
	UnloadImage(mImage);
}

void TestScene::OnUpdate(float dt)
{
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
			Color3 finalColor = Vector3Zero();

			for (float k = 0.0f; k < 4.0f; k++)
			{
				uv.x = Fract(uv.x * 1.5f) - 0.5f;
				uv.y = Fract(uv.y * 1.5f) - 0.5f;

				Color3 col = Palette(Length(uv0) + k * 0.4f + tt * 0.4f);
				float d = Length(uv);
				d = sinf(d * 8.0f + tt) / 8.0f;
				d = fabsf(d);
				d = powf(0.01f / d, 1.0f);

				finalColor += col * d;
			}

			Color color = Convert(finalColor);
			SetPixel(mImage, x, y, color);
		}
	}
}

void TestScene::OnDraw()
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
