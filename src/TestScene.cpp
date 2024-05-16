#include "TestScene.h"
#include "Shader.h"
#include "App.h"

float Fract(float f)
{
	return f - floorf(f);
}

Color3 Palette(float t) {
	Color3 a { 0.5f, 0.5f, 0.5f };
	Color3 b { 0.5f, 0.5f, 0.5f };
	Color3 c { 1.0f, 1.0f, 1.0f };
	Color3 d{ 0.263f, 0.416f, 0.557f };

	Color3 cd = (c * t + d) * 6.28318f;
	cd.r = cosf(cd.r);
	cd.g = cosf(cd.g);
	cd.b = cosf(cd.b);

	return a + b * cd;
}

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
	float tt = TotalTime();
	float ncos = cosf(tt) * 0.5f + 0.5f;
	Color3 bg = Palette(ncos);

	for (int i = 0; i < mImage.height; i++)
	{
		for (int j = 0; j < mImage.width; j++)
		{
			Vector2 fragCoord{ j, i };
			Vector2 resolution{ mImage.width, mImage.height };
			Vector2 uv = fragCoord / resolution;
			uv = uv * 2.0f - 1.0f;
			uv.x *= SCREEN_ASPECT;

			uv.x = Fract(uv.x * 2.0f);
			uv.y = Fract(uv.y * 2.0f);

			Color color = Length(uv) > 1.0 ? WHITE : BLACK;
			mImage.pixels[i * mImage.width + j] = color;
		}
	}
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
