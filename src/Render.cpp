#include "Render.h"
#include <par_shapes.h>

// File-scope cause we probably shouldn't make these global.
// This is all most-likely temporary until we get some sort of camera system.
Matrix fView = MatrixIdentity();
Matrix fProj = MatrixIdentity();

void SetView(Matrix view)
{
	fView = view;
}

void SetProj(Matrix proj)
{
	fProj = proj;
}

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
	GenSquare(mesh);

	Matrix world =
		Scale(width, height, 1.0f) *
		RotateZ(angle) *
		Translate(center.x, center.y, 0.0f);
	
	Matrix mvp = world * fView * fProj;
	DrawMeshFlat(mesh, mvp, color);
	DestroyMesh(mesh);
}

void DrawTriangle(Vector2 v0, Vector2 v1, Vector2 v2, Vector3 color, float angle)
{
	Mesh mesh;
	GenTriangle(mesh, v0, v1, v2);

	Matrix mvp = RotateZ(angle) * fView * fProj;
	DrawMeshFlat(mesh, mvp, color);
	DestroyMesh(mesh);
}

void DrawCircle(Vector2 center, float radius, Vector3 color, float angle)
{
	Mesh mesh;
	GenCircle(mesh);

	Matrix world =
		Scale(radius, radius, 1.0f) *
		RotateZ(angle) * 
		Translate(center.x, center.y, 0.0f);

	Matrix mvp = world * fView * fProj;
	DrawMeshFlat(mesh, mvp, color);
	DestroyMesh(mesh);
}

// GenSemicircle produces a vertical semicircle
void DrawSemicircle(Vector2 center, float radius, Vector3 color, float angle)
{
	Mesh mesh;
	GenSemicircle(mesh);

	Matrix world =
		Scale(radius, radius, 1.0f) *
		RotateZ(angle) *
		Translate(center.x, center.y, 0.0f);

	Matrix mvp = world * fView * fProj;
	DrawMeshFlat(mesh, mvp, color);
	DestroyMesh(mesh);
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

void DrawCube(Vector3 center, float width, float height, float depth, Vector3 color, Matrix rotation)
{
	Mesh mesh;
	GenCube(mesh);

	Matrix world =
		Scale(width, height, depth) * 
		rotation * 
		Translate(center.x, center.y, center.z);

	Matrix mvp = world * fView * fProj;
	DrawMeshFlat(mesh, mvp, color);
	DestroyMesh(mesh);
}

void DrawSphere(Vector3 center, float radius, Vector3 color, Matrix rotation)
{
	Mesh mesh;
	GenSphere(mesh);

	Matrix world =
		Scale(radius, radius, radius) *
		rotation * 
		Translate(center.x, center.y, center.z);

	Matrix mvp = world * fView * fProj;
	DrawMeshFlat(mesh, mvp, color);
	DestroyMesh(mesh);
}

void DrawHemisphere(Vector3 center, float radius, Vector3 color, Matrix rotation)
{
	Mesh mesh;
	GenHemisphere(mesh);

	Matrix world =
		Scale(radius, radius, radius) *
		rotation *
		Translate(center.x, center.y, center.z);

	Matrix mvp = world * fView * fProj;
	DrawMeshFlat(mesh, mvp, color);
	DestroyMesh(mesh);
}

void DrawCylinder(Vector3 center, float radius, float halfHeight, Vector3 color, Matrix rotation)
{
	Mesh mesh;
	GenCylinder(mesh);

	Matrix world =
		Scale(radius, radius, halfHeight) *
		rotation *
		Translate(center.x, center.y, center.z);

	Matrix mvp = world * fView * fProj;
	DrawMeshFlat(mesh, mvp, color);
	DestroyMesh(mesh);
}

void DrawSpherocylinder(Vector3 center, float radius, float halfHeight, Vector3 color, Matrix rotation)
{
	// TODO -- complete this!
}

void DrawPlaneXZ(Vector3 center, float width, float depth, Vector3 color, Matrix rotation)
{
	Mesh mesh;
	GenPlaneXZ(mesh);

	Matrix world =
		Scale(width, 1.0f, depth) *
		rotation *
		Translate(center.x, center.y, center.z);

	Matrix mvp = world * fView * fProj;
	DrawMeshFlat(mesh, mvp, color);
	DestroyMesh(mesh);
}

void DrawPlaneYZ(Vector3 center, float height, float depth, Vector3 color, Matrix rotation)
{
	Mesh mesh;
	GenPlaneYZ(mesh);

	Matrix world =
		Scale(1.0f, height, depth) *
		rotation *
		Translate(center.x, center.y, center.z);

	Matrix mvp = world * fView * fProj;
	DrawMeshFlat(mesh, mvp, color);
	DestroyMesh(mesh);
}

void DrawPlaneXY(Vector3 center, float width, float height, Vector3 color, Matrix rotation)
{
	Mesh mesh;
	GenPlaneXY(mesh);

	Matrix world =
		Scale(width, height, 1.0f) *
		rotation *
		Translate(center.x, center.y, center.z);

	Matrix mvp = world * fView * fProj;
	DrawMeshFlat(mesh, mvp, color);
	DestroyMesh(mesh);
}

void DrawDodecahedron(Vector3 center, Vector3 scale, Vector3 color, Matrix rotation)
{
	Mesh mesh;
	GenDodecahedron(mesh);

	Matrix world =
		Scale(scale) *
		rotation *
		Translate(center.x, center.y, center.z);

	Matrix mvp = world * fView * fProj;
	DrawMeshFlat(mesh, mvp, color);
	DestroyMesh(mesh);
}
