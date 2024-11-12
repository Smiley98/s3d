#pragma once
#include "Scene.h"
#include "Image.h"
#include "Texture.h"
#include "Mesh.h"

class MainScene : public Scene
{
public:
	void OnLoad() final;
	void OnUnload() final;

	void OnUpdate(float dt) final;
	void OnDraw() final;

private:
	Image mImage;
	Texture mTexture;
	Mesh mMesh;
};