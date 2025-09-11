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

	DrawCircle(&gImageCPU, 100, 100, 10, MAGENTA);		// Filled circle
	DrawCircleLines(&gImageCPU, 100, 100, 11, CYAN);	// Outlined circle

	// Homework:
	// Draw a border around your screen using DrawRectLines
	// Draw 2 horizontal lines
	// Draw 2 vertical lines
	// Draw a rectangle in the centre of your screen and outline it
	// Draw a circle at your mouse cursor and outline it
	// Draw a line connecting the above two shapes
}
