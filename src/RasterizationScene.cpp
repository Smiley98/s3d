#include "RasterizationScene.h"
#include "Render.h"

void RasterizationScene::OnDraw()
{
	DrawMeshWireframes(gMeshSphere, MatrixIdentity(), { 1.0f, 0.0f, 0.0f });
}
