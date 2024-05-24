#include "MainScene.h"
#include "Shader.h"
#include "Mesh.h"

void MainScene::OnUpdate(float dt)
{
}

void MainScene::OnDraw()
{
	GLint col = glGetUniformLocation(gShaderColor, "u_color");
	GLint mvp = glGetUniformLocation(gShaderColor, "u_mvp");

	Matrix m = MatrixIdentity();

	BindShader(gShaderColor);
	glUniformMatrix4fv(mvp, 1, GL_FALSE, &m.m0);
	glUniform3f(col, 1.0f, 0.0f, 0.0f);
	DrawMesh(gMeshTriangle);
	UnbindShader();
}

void MainScene::OnDrawGui()
{
}
