#include "RasterizationScene.h"
#include "Shader.h"
#include "Mesh.h"

void RasterizationScene::OnDraw()
{
	Matrix m = MatrixIdentity();
	BindShader(&gShaderColor);
	SendMat4("u_mvp", &m);
	SendVec3("u_color", { 1.0f, 0.0f, 0.0f });
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	DrawMesh(gMeshSphere);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	UnbindShader();
}
