#include "TestScene.h"
#include "Shader.h"
#include "Mesh.h"
#include "App.h"
#include "ImageUtils.h"

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
	for (int y = 0; y < mImage.height; y++)
	{
		for (int x = 0; x < mImage.width; x++)
		{
			Vector2 fragCoord{ x, y };
			Vector2 resolution{ mImage.width, mImage.height };
			Vector2 uv = fragCoord / resolution;
			uv = uv * 2.0f - 1.0f;
			uv.x *= SCREEN_ASPECT;
			float dist = Length(uv);

			Color color = dist > 1.0f ? WHITE : BLACK;
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
