#include "Render.h"
#include <par_shapes.h>

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
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	DrawMeshFlat(mesh, mvp, color);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void DrawMeshPositions(Mesh mesh, Matrix mvp, Matrix world)
{
	BindShader(&gShaderPositions);
	SendMat4("u_mvp", &mvp);
	SendMat4("u_world", &world);
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
