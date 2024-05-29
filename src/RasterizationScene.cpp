#include "RasterizationScene.h"
#include "Shader.h"
#include "Mesh.h"
#include "Rasterization.h"
#include <cstdio>
#include <algorithm>

void RasterizationScene::OnLoad()
{
	LoadImage(mImage, 512, 512);
	LoadTexture(&mTexture, 512, 512);
}

void RasterizationScene::OnUnload()
{
	UnloadTexture(&mTexture);
	UnloadImage(mImage);
}

void RasterizationScene::OnUpdate(float dt)
{
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
