#include "RasterizationScene.h"
#include "Rasterization.h"
#include "ImageUtils.h"
#include "Camera.h"
#include "Time.h"

void RasterizationScene::OnLoad()
{
}

void RasterizationScene::OnUnload()
{
}

void RasterizationScene::OnUpdate(float dt)
{
	ClearColor(&gImageCPU, BLACK);
	DrawMesh(&gImageCPU, gMeshHead);
	// works with obj meshes but not par_shapes meshes since par_shapes uses index buffer
}
