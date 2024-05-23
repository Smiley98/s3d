#include "DDAScene.h"
#include "Shader.h"
#include "Mesh.h"

void DDAScene::OnLoad()
{
	LoadImage(mImage, 512, 512);
	mTexture = LoadTexture(mImage);
}

void DDAScene::OnUnload()
{
	UnloadTexture(mTexture);
	UnloadImage(mImage);
}

void DDAScene::OnUpdate(float dt)
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

			Color color = Length(uv) > 1.0f ? WHITE : BLACK;
			SetPixel(mImage, x, y, color);
		}
	}
	UpdateTexture(mTexture, mImage);
}

void DDAScene::OnDraw()
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
