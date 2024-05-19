#include "RasterizationScene.h"
#include "Shader.h"
#include "Mesh.h"
#include <cstdio>
#include <algorithm>

void RasterizationScene::OnLoad()
{
	LoadImage(mImage, 512, 512);
	mTexture = LoadTexture(mImage);
}

void RasterizationScene::OnUnload()
{
	UnloadTexture(mTexture);
	UnloadImage(mImage);
}

void RasterizationScene::OnUpdate(float dt)
{
	//for (int y = 0; y < mImage.height; y++)
	//{
	//	for (int x = 0; x < mImage.width; x++)
	//	{
	//		Vector2 fragCoord{ x, y };
	//		Vector2 resolution{ mImage.width, mImage.height };
	//		Vector2 uv = fragCoord / resolution;
	//		//uv = uv * 2.0f - 1.0f;
	//		//uv = uv * 0.5f + 0.5f;
	//		uv.x *= SCREEN_ASPECT;
	//		Color color = Convert(Color4{ uv.x, uv.y, 0.0f, 1.0f });
	//		SetPixel(mImage, x, y, color);
	//	}
	//}

	for (int x = 0; x < mImage.width; x++)
	{
		DrawLine(mImage, x, 0, x, mImage.height - 1, RED);
	}

	DrawRect(mImage, 100, 50, 100, 50, GREEN);
}

void RasterizationScene::OnDraw()
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
