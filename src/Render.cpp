#include "Render.h"
#include <par_shapes.h>

Matrix gView = MatrixIdentity();
Matrix gProj = MatrixIdentity();

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

void DrawRectangle(Vector2 center, float width, float height, Vector3 color, float angle)
{
	Mesh mesh;
	GenPlane(&mesh);

	Matrix world =
		Translate(-0.5f, -0.5f, 0.0f) *
		Scale(width, height, 1.0f) *
		RotateZ(angle) *
		Translate(center.x, center.y, 0.0f);
	
	Matrix mvp = world * gView * gProj;
	DrawMeshFlat(mesh, mvp, color);

	DestroyMesh(&mesh);
}

void DrawCircle(Vector2 center, float radius, Vector3 color)
{
	Mesh mesh;
	GenCircle(&mesh);

	Matrix world =
		Scale(radius, radius, 1.0f) *
		Translate(center.x, center.y, 0.0f);

	Matrix mvp = world * gView * gProj;
	DrawMeshFlat(mesh, mvp, color);

	DestroyMesh(&mesh);
}

// GenSemicircle produces a vertical semicircle
void DrawSemicircle(Vector2 center, float radius, Vector3 color, float angle)
{
	Mesh mesh;
	GenSemicircle(&mesh);

	Matrix world =
		Scale(radius, radius, 1.0f) *
		RotateZ(angle) *
		Translate(center.x, center.y, 0.0f);

	Matrix mvp = world * gView * gProj;
	DrawMeshFlat(mesh, mvp, color);

	DestroyMesh(&mesh);
}

void DrawSemicircleH(Vector2 center, float radius, Vector3 color, float angle)
{
	DrawSemicircle(center, radius, color, angle - PI * 0.5f);
}

void DrawSemicircleV(Vector2 center, float radius, Vector3 color, float angle)
{
	DrawSemicircle(center, radius, color, angle);
}

void DrawCapsuleH(Vector2 center, float radius, float halfHeight, Vector3 color, float angle)
{
	Vector2 direction = Rotate(V2_RIGHT, angle);
	Vector2 top = center + direction * halfHeight;
	Vector2 bot = center - direction * halfHeight;
	DrawSemicircleH(top, radius, color, angle);
	DrawSemicircleH(bot, radius, color, angle + PI);
	DrawRectangle(center, halfHeight * 2.0f, radius * 2.0f, color, angle);
}

void DrawCapsuleV(Vector2 center, float radius, float halfHeight, Vector3 color, float angle)
{
	Vector2 direction = Rotate(V2_UP, angle);
	Vector2 top = center + direction * halfHeight;
	Vector2 bot = center - direction * halfHeight;
	DrawSemicircleV(top, radius, color, angle);
	DrawSemicircleV(bot, radius, color, angle + PI);
	DrawRectangle(center, radius * 2.0f, halfHeight * 2.0f, color, angle);
}
