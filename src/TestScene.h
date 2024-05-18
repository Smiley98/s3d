#pragma once
#include "Scene.h"
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
	GLuint mTexture;
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
	GLuint mTexture;
};

void TestScene::OnLoad()
{
	LoadImage(mImage, 512, 512);
	mTexture = LoadTexture(mImage);
}

void TestScene::OnUnload()
{
	UnloadTexture(mTexture);
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
	UpdateTexture(mTexture, mImage);
}
*/
