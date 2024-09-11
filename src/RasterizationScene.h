#pragma once
#include "Scene.h"
#include "Render.h"

class RasterizationScene : public Scene
{
public:
	void OnCreate() final;
	void OnDestroy() final;

	void OnDraw() final;
	void OnDrawImGui() final;

private:
	Shader* mShader = nullptr;
	Mesh* mMesh = &gMeshTriangle;

	Matrix mView;
	Matrix mProj;
};
