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
			Color color = RED;
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
