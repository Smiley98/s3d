#include "RasterizationScene.h"
#include "Render.h"
#include "Time.h"

void RasterizationScene::OnDraw()
{
	float tt = TotalTime();
	float nsin = sinf(tt) * 0.5f + 0.5f;
	float ncos = cosf(tt) * 0.5f + 0.5f;

	Matrix translation = MatrixIdentity();//Translate(cos(tt), 0.0f, 0.0f);
	Matrix rotation = RotateY(tt * 100.0f * DEG2RAD);
	Matrix scale = MatrixIdentity();//Scale(cos(tt) * 0.4f + 0.6f, sin(tt) * 0.4f + 0.6f, 1.0f);

	Matrix world = scale * rotation * translation;
	Matrix view = MatrixIdentity();//LookAt({ 0.0f, 0.0f, 5.0f }, V3_ZERO, V3_UP);
	Matrix proj = MatrixIdentity();//Perspective(75.0f * DEG2RAD, 1.0f, 0.001f, 100.0f);
	Matrix mvp = world * view * proj;

	DrawMeshWireframes(gMeshTriangle, mvp, { 1.0f, 0.0f, 0.0f });
	DrawMeshWireframes(gMeshCube, mvp, { 0.0f, 1.0f, 0.0f });
	DrawMeshWireframes(gMeshSphere, mvp, { 0.0f, 0.0f, 1.0f });
	//DrawMeshNormals(gMeshCube, mvp, world);
}
