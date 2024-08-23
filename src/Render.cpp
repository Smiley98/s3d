#include "Render.h"

void DrawMeshWireframes(Mesh mesh, Matrix mvp, Vector3 color)
{
	BindShader(&gShaderColor);
	SendMat4("u_mvp", &mvp);
	SendVec3("u_color", color);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	DrawMesh(mesh);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
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
