#include "RasterizationScene.h"
#include "Shader.h"
#include "Mesh.h"
#include <cstdio>

Image fImage;
GLuint fTexture;

// Loses precision on horizontal lines, so just loop manually in that case!
void Line(int x0, int y0, int x1, int y1, Color color)
{
	for (int x = x0; x <= x1; x++)
	{
		// Calculate interpolation value
		float t = (x - x0) / (float)(x1 - x0);

		// Express y in terms of x by lerping!
		int y = y0 * (1.0f - t) + y1 * t;

		SetPixel(fImage, x, y, color);
	}
}

void RasterizationScene::OnLoad()
{
	LoadImage(fImage, 512, 512);
	fTexture = LoadTexture(fImage);
}

void RasterizationScene::OnUnload()
{
	UnloadTexture(fTexture);
	UnloadImage(fImage);
}

void RasterizationScene::OnUpdate(float dt)
{
	// If we DONT flip the image, it behaves as if [0, 0] is the origin,
	// which is what we want! Flip if we want [0, 0] in the top-left.
	// Proof -- look at the following UV demo:
	for (int y = 0; y < fImage.height; y++)
	{
		//for (int x = 0; x < fImage.width; x++)
		//{
		//	Vector2 fragCoord{ x, y };
		//	Vector2 resolution{ fImage.width, fImage.height };
		//	Vector2 uv = fragCoord / resolution;
		//	//uv = uv * 2.0f - 1.0f;
		//	//uv = uv * 0.5f + 0.5f;
		//	uv.x *= SCREEN_ASPECT;
		//	Color color = Convert(Color4{ uv.x, uv.y, 0.0f, 1.0f });
		//	SetPixel(fImage, x, y, color);
		//}
		
		//Line(0, y, fImage.width - 1, y, RED); <-- precision errors xD xD xD
		if (y % 2 == 0)
			SetRow(fImage, y, RED);
	}

	// Need to swap x & y to handle vertical line divide-by-zero	
	for (int x = 0; x < fImage.width; x++)
	{
		//Line(x, 0, x, fImage.height - 1, RED); <-- divide by zero xD xD xD
		if (x % 2 == 1)
			SetCol(fImage, x, GREEN);
	}
}

void RasterizationScene::OnDraw()
{
	UpdateTexture(fTexture, fImage);

	BindTexture(fTexture);
	glDepthMask(GL_FALSE);
	glUseProgram(gShaderFSQ);
	glUniform1i(glGetUniformLocation(gShaderFSQ, "u_tex"), 0);
	BindFsq();
	DrawFsq();

	glBindVertexArray(GL_NONE);
	glUseProgram(GL_NONE);
	glDepthMask(GL_TRUE);
	UnbindTexture(fTexture);
}
