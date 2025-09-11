#include "RasterizationScene.h"
#include "Rasterization.h"
#include "ImageUtils.h"
#include "Camera.h"
#include "Time.h"

static Image fDiffuseMap;

void RasterizationScene::OnLoad()
{
	gCamera = FromView(LookAt({ 0.0f, 0.0f, 5.0f }, V3_ZERO, V3_UP));
	CreateImageFromFile(&fDiffuseMap, "assets/textures/african_head_diffuse.png", true);
}

void RasterizationScene::OnUnload()
{
	DestroyImage(&fDiffuseMap);
}

void RasterizationScene::OnUpdate(float dt)
{
	ClearColor(&gImageCPU, BLACK);
	// TODO - loop through every pixel in the image and set each pixel's colour!
}
