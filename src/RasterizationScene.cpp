#include "RasterizationScene.h"
#include "Render.h"
#include "Time.h"

void RasterizationScene::OnDraw()
{
	Matrix translation = MatrixIdentity();
	Matrix rotation = RotateY(TotalTime() * 100.0f * DEG2RAD);
	Matrix scale = MatrixIdentity();

	Matrix world = scale * rotation * translation;
	Matrix view = LookAt({ 0.0f, 0.0f, 1.5f }, V3_ZERO, V3_UP);
	Matrix proj = Ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 2.0f);
	//Matrix proj = Perspective(75.0f * DEG2RAD, 1.0f, 0.001f, 100.0f);
	Matrix mvp = world * view * proj;

	//DrawMeshWireframes(gMeshTriangle, mvp, { 1.0f, 0.0f, 0.0f });
	//DrawMeshWireframes(gMeshPlane, mvp, { 1.0f, 0.0f, 1.0f });
	//DrawMeshWireframes(gMeshCube, mvp, { 0.0f, 1.0f, 0.0f });
	//DrawMeshWireframes(gMeshCircle, mvp, { 1.0f, 1.0f, 0.0f });
	//DrawMeshWireframes(gMeshSphere, mvp, { 0.0f, 0.0f, 1.0f });
	//DrawMeshWireframes(gMeshHead, mvp, { 0.0f, 1.0f, 1.0f });

	DrawMeshPositionsWorld(gMeshHead, mvp, world);
	SetWireframes(true);
	DrawMeshPositionsScreen(gMeshHead, mvp);
	SetWireframes(false);

	//DrawMeshNormals(gMeshTriangle, mvp, world);
	//DrawMeshNormals(gMeshPlane, mvp, world);
	//DrawMeshNormals(gMeshCube, mvp, world);
	//DrawMeshNormals(gMeshCircle, mvp, world);
	//DrawMeshNormals(gMeshSphere, mvp, world);
	//DrawMeshNormals(gMeshHead, mvp, world);

	// TODO -- create sphere, capsule, and box rendering functions via par_shapes
}
