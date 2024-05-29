#pragma once
#include "Scene.h"
#include "Image.h"
#include "Texture.h"

class TestScene : public Scene
{
public:
	void OnLoad() final;
	void OnUnload() final;

	void OnUpdate(float dt) final;
	void OnDraw() final;

private:
	Image mImage;
	Texture mTexture;
};

// Setup for CPU rendering:
/*
class TestScene : public Scene
{
public:
	void OnLoad() final;
	void OnUnload() final;

	void OnUpdate(float dt) final;
	void OnDraw() final;

private:
	Image mImage;
	Texture mTexture;
};

void TestScene::OnLoad()
{
	LoadImage(mImage, 512, 512);
	LoadTexture(&mTexture, 512, 512);
}

void TestScene::OnUnload()
{
	UnloadTexture(&mTexture);
	UnloadImage(mImage);
}

void TestScene::OnUpdate(float dt)
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
}

void TestScene::OnDraw()
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
*/
