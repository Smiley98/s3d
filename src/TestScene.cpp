#include "TestScene.h"
#include "Shader.h"

void TestScene::OnLoad()
{
	glGenVertexArrays(1, &mVaoFsq);
	LoadImage(mImage, 64, 64);
	mTexture = LoadTexture(mImage);
}

void TestScene::OnUnload()
{
	UnloadTexture(mTexture);
	UnloadImage(mImage);
}

void TestScene::OnUpdate(float dt)
{
	for (int i = 0; i < mImage.height; i++)
	{
		for (int j = 0; j < mImage.width; j++)
		{
			Color2 uv = { j / (float)mImage.width, i / (float)mImage.height };
			uv.r = uv.r * 2.0f - 1.0f;
			uv.g = uv.g * 2.0f - 1.0f;

			Color color = Convert(uv);
			color.a = 255;

			//Color colors[] = { RED, GREEN, BLUE };
			mImage.pixels[i * mImage.width + j] = color;
		}
	}
	Flip(mImage);
	UpdateTexture(mTexture, mImage);
}

void TestScene::OnDraw()
{
	BindTexture(mTexture);
	glDepthMask(GL_FALSE);
	glUseProgram(gShaderFSQ);
	glUniform1i(glGetUniformLocation(gShaderFSQ, "u_tex"), 0);
	glBindVertexArray(mVaoFsq);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	glBindVertexArray(GL_NONE);
	glUseProgram(GL_NONE);
	glDepthMask(GL_TRUE);
	UnbindTexture(mTexture);
}
