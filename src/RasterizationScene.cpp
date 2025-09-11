#include "RasterizationScene.h"
#include "Rasterization.h"
#include "ImageUtils.h"
#include "Camera.h"
#include "Time.h"
#include <vector>

void RasterizationScene::OnLoad()
{
}

void RasterizationScene::OnUnload()
{
}

void RasterizationScene::OnUpdate(float dt)
{
	ClearColor(&gImageCPU, BLACK);

	// Select our mesh and create a copy of its original vertex positions
	Mesh& mesh = gMeshCube;
	std::vector<Vector3> positions;
	positions.resize(mesh.count);

	// Transform the copied positions to rotate continuously around the Y-axis
	Matrix rotation = RotateY(TotalTime() * 100.0f * DEG2RAD);
	for (size_t i = 0; i < mesh.count; i++)
	{
		int index = mesh.indices[i];
		positions[i] = rotation * mesh.positions[index];
	}
	
	// Loop through all faces of our mesh and render them as wireframes!
	for (size_t i = 0; i < mesh.count / 3; i++)
		DrawFaceWireframes(&gImageCPU, positions.data(), i, GREEN);
}
