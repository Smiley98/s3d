#include "RasterizationScene.h"
#include "Shader.h"
#include "Mesh.h"

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
	for (int i = 0; i < mImage.height; i++)
	{
		for (int j = 0; j < mImage.width; j++)
		{
			Vector2 fragCoord{ j, i };
			Vector2 resolution{ mImage.width, mImage.height };
			Vector2 uv = fragCoord / resolution;
			uv = uv * 2.0f - 1.0f;
			uv.x *= SCREEN_ASPECT;

			Color color = Length(uv) > 1.0f ? WHITE : BLACK;
			mImage.pixels[i * mImage.width + j] = color;
		}
	}
	UpdateTexture(mTexture, mImage);
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
