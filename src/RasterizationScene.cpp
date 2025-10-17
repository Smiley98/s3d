#include "RasterizationScene.h"
#include "Rasterization.h"
#include "ImageUtils.h"
#include "Camera.h"
#include "Time.h"

static Image texture;

void RasterizationScene::OnLoad()
{
	CreateImageFromFile(&texture, "./assets/textures/african_head_diffuse.png", true);
}

void RasterizationScene::OnUnload()
{
	DestroyImage(&texture);
}

void RasterizationScene::OnUpdate(float dt)
{
	ClearColor(&gImageCPU, BLACK);
	ClearDepth(&gImageCPU, 1.0f);
	float tt = TotalTime();

	Matrix world = Translate(0.0f, 0.0f, 5.0f + sinf(tt) * 3.0f);
	Matrix view = LookAt({ 0.0f, 0.0f, 10.0f }, V3_ZERO, V3_UP);
	Matrix proj = Perspective(90.0f * DEG2RAD, 1.0f, 0.1f, 10.0f);
	Matrix mvp = world * view * proj;

	UniformData data;
	data.world = world;
	data.mvp = mvp;
	data.texture = &texture;

	DrawMesh(&gImageCPU, gMeshHead, data);
}
