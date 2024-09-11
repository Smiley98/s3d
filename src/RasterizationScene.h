#pragma once
#include "Scene.h"
#include "Render.h"

class RasterizationScene : public Scene
{
public:
	void OnDraw() final;
	void OnDrawImGui() final;

private:
	Shader* mShader = nullptr;
	Mesh* mMesh = &gMeshHead;

	Matrix mView;
	Matrix mProj;
};
