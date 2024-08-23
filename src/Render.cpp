#include "Render.h"

void DrawMeshWireframes(Mesh mesh, Matrix mvp, Vector3 color)
{
	//Matrix normal = NormalMatrix()
	BindShader(&gShaderColor);
	SendMat4("u_mvp", &mvp);
	SendVec3("u_color", color);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	DrawMesh(mesh);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	UnbindShader();
}
