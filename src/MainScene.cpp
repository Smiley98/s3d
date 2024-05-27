#include "MainScene.h"
#include "Shader.h"
#include "Mesh.h"
#include "App.h"


void MainScene::OnUpdate(float dt)
{
}

void MainScene::OnDraw()
{
	float tt = TotalTime();

	GLint col = glGetUniformLocation(gShaderColor, "u_color");
	GLint mvp = glGetUniformLocation(gShaderColor, "u_mvp");

	Matrix m = MatrixIdentity();//Scale(0.5f, 0.5f, 0.5f) * RotateY(100.0f * tt * DEG2RAD);
	m = Transpose(m);

	BindShader(gShaderColor);
	glUniformMatrix4fv(mvp, 1, GL_FALSE, &m.m0);
	glUniform3f(col, 1.0f, 0.0f, 0.0f);
	//DrawMesh(gMeshTriangle);
	DrawMesh(gMeshCube);

	//bool cull = glIsEnabled(GL_CULL_FACE);	// true
	//int windingOrder;
	//glGetIntegerv(GL_FRONT_FACE, &windingOrder);
	//bool isCCW = windingOrder == GL_CCW;		// true
	UnbindShader();
}

void MainScene::OnDrawGui()
{
}
