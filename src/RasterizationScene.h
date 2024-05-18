#pragma once
#include "Scene.h"
#include "Texture.h"

class RasterizationScene : public Scene
{
public:
	void OnLoad() final;
	void OnUnload() final;

	void OnUpdate(float dt) final;
	void OnDraw() final;

private:
	Image mImage;
	GLuint mTexture;

	void Line(int x0, int y0, int x1, int y1, Color color);
};
