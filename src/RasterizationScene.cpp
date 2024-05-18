#include "RasterizationScene.h"
#include "Shader.h"
#include "Mesh.h"
#include <cstdio>
#include <algorithm>

// Loses precision on horizontal lines, so just loop manually in that case!
void RasterizationScene::Line(int x0, int y0, int x1, int y1, Color color)
{
	// Make x the smaller of x vs y for more accuracy when interpolating
	bool steep = false;
	if (fabsf(x0 - x1) < fabsf(y0 - y1))
	{
		steep = true;
		std::swap(x0, y0);
		std::swap(x1, y1);
	}

	// Make the line always points left-to-right
	if (x0 > x1)
	{
		std::swap(x0, x1);
		std::swap(y0, y1);
	}

	for (int x = x0; x <= x1; x++)
	{
		// Calculate interpolation value
		float t = (x - x0) / (float)(x1 - x0);

		// Express y in terms of x by lerping!
		int y = y0 * (1.0f - t) + y1 * t;

		int px = steep ? y : x;
		int py = steep ? x : y;
		SetPixel(mImage, px, py, color);
	}
}

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
		Line(x, 0, x, mImage.height - 1, RED);
	}
}

void RasterizationScene::OnDraw()
{
	UpdateTexture(mTexture, mImage);

	BindTexture(mTexture);
	glDepthMask(GL_FALSE);
	glUseProgram(gShaderFSQ);
	glUniform1i(glGetUniformLocation(gShaderFSQ, "u_tex"), 0);
	BindFsq();
	DrawFsq();

	glBindVertexArray(GL_NONE);
	glUseProgram(GL_NONE);
	glDepthMask(GL_TRUE);
	UnbindTexture(mTexture);
}
