#include "Render.h"
#include <par_shapes.h>

void SetWireframes(bool wireframes)
{
	glPolygonMode(GL_FRONT_AND_BACK, wireframes ? GL_LINE : GL_FILL);
}

void DrawMeshFlat(Mesh mesh, Matrix mvp, Vector3 color)
{
	BindShader(&gShaderColor);
	SendMat4("u_mvp", &mvp);
	SendVec3("u_color", color);
	DrawMesh(mesh);
	UnbindShader();
}

void DrawMeshWireframes(Mesh mesh, Matrix mvp, Vector3 color)
{
	SetWireframes(true);
	DrawMeshFlat(mesh, mvp, color);
	SetWireframes(false);
}

void DrawMeshPositionsWorld(Mesh mesh, Matrix mvp, Matrix world)
{
	BindShader(&gShaderPositionsWorld);
	SendMat4("u_mvp", &mvp);
	SendMat4("u_world", &world);
	DrawMesh(mesh);
	UnbindShader();
}

void DrawMeshPositionsScreen(Mesh mesh, Matrix mvp, Vector2 resolution)
{
	BindShader(&gShaderPositionsScreen);
	SendMat4("u_mvp", &mvp);
	SendVec2("u_resolution", resolution);
	DrawMesh(mesh);
	UnbindShader();
}

void DrawMeshNormals(Mesh mesh, Matrix mvp, Matrix world)
{
	Matrix normal = NormalMatrix(world);
	BindShader(&gShaderNormals);
	SendMat4("u_mvp", &mvp);
	SendMat3("u_normal", &normal);
	DrawMesh(mesh);
	UnbindShader();
}
