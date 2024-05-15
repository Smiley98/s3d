#include "TestScene.h"
#include "Shader.h"

void TestScene::OnLoad()
{
	glGenVertexArrays(1, &mVaoFsq);

	Image image;
	LoadImage(image, 64, 64);
	for (int i = 0; i < image.height; i++)
	{
		for (int j = 0; j < image.width; j++)
		{
			Color2 uv = { j / (float)image.width, i / (float)image.height };
			uv.r = uv.r * 2.0f - 1.0f;
			uv.g = uv.g * 2.0f - 1.0f;

			Color color = Convert(uv);
			color.a = 255;

			//Color colors[] = { RED, GREEN, BLUE };
			image.pixels[i * image.width + j] = color;
		}
	}
	Flip(image);
	mTexture = LoadTexture(image);
	UnloadImage(image);
}

void TestScene::OnUnload()
{
	UnloadTexture(mTexture);
}

void TestScene::OnUpdate(float dt)
{
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
