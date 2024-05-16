#include "TestScene.h"
#include "Shader.h"

void TestScene::OnLoad()
{
	glGenVertexArrays(1, &mVaoFsq);
	LoadImage(mImage, 256, 256);
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
			Vector2 fragCoord{ j, i };
			Vector2 resolution{ mImage.width, mImage.height };
			Vector2 uv = fragCoord / resolution;
			uv = uv * 2.0f - 1.0f;
			uv.x *= SCREEN_ASPECT;

			float distance = Length(uv);
			Color color = distance > 1.0f ? WHITE : BLACK;
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
