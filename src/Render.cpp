#include "Render.h"
#include <par_shapes.h>

DebugShaderType gDebugShader = FLAT;
Matrix gView = MatrixIdentity();
Matrix gProj = MatrixIdentity();

void SetDebugShader(DebugShaderType type)
{
	gDebugShader = type;
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

void DrawMeshTcoords(Mesh mesh, Matrix mvp)
{
	if (mesh.tcoords != nullptr)
	{
		BindShader(&gShaderTcoords);
		SendMat4("u_mvp", &mvp);
		DrawMesh(mesh);
		UnbindShader();
	}
}

void DrawMeshTexture(Mesh mesh, Matrix mvp, Matrix world, Texture texture)
{
	assert(mesh.tcoords != nullptr);
	Matrix normal = NormalMatrix(world);
	BindTexture(texture);
	BindShader(&gShaderTexture);
	SendMat4("u_mvp", &mvp);
	SendMat3("u_normal", &normal);
	SendInt("u_tex", 0);
	DrawMesh(mesh);
	UnbindShader();
	UnbindTexture(texture);
}

void DrawRectangle(Vector2 center, float width, float height, Vector3 color, float angle)
{
	Mesh mesh;
	GenSquare(mesh);

	Matrix world =
		Scale(width, height, 1.0f) *
		RotateZ(angle) *
		Translate(center.x, center.y, 0.0f);
	
	Matrix mvp = world * gView * gProj;
	DrawMeshDebug(mesh, mvp, world, color);
	DestroyMesh(mesh);
}

void DrawTriangle(Vector2 v0, Vector2 v1, Vector2 v2, Vector3 color, float angle)
{
	Mesh mesh;
	GenTriangle(mesh, v0, v1, v2);

	Matrix mvp = RotateZ(angle) * gView * gProj;
	DrawMeshDebug(mesh, mvp, MatrixIdentity(), color);
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

	Matrix mvp = world * gView * gProj;
	DrawMeshDebug(mesh, mvp, world, color);
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

	Matrix mvp = world * gView * gProj;
	DrawMeshDebug(mesh, mvp, world, color);
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

	Matrix mvp = world * gView * gProj;
	DrawMeshDebug(mesh, mvp, world, color);
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

	Matrix mvp = world * gView * gProj;
	DrawMeshDebug(mesh, mvp, world, color);
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

	Matrix mvp = world * gView * gProj;
	DrawMeshDebug(mesh, mvp, world, color);
	DestroyMesh(mesh);
}

void DrawCylinder(Vector3 center, float radius, float halfHeight, Vector3 color, Matrix rotation)
{
	Mesh mesh;
	GenCylinder(mesh);

	Matrix world =
		Scale(radius, radius, halfHeight * 2.0f) *
		rotation *
		Translate(center.x, center.y, center.z);

	Matrix mvp = world * gView * gProj;
	DrawMeshDebug(mesh, mvp, world, color);
	DestroyMesh(mesh);
}

void DrawSpherocylinder(Vector3 center, float radius, float halfHeight, Vector3 color, Matrix rotation)
{
	Vector3 forward = Forward(rotation);
	Vector3 top = center + forward * halfHeight;
	Vector3 bot = center - forward * halfHeight;
	
	DrawCylinder(center, radius, halfHeight, color, rotation);
	DrawHemisphere(top, radius, color, rotation);
	DrawHemisphere(bot, radius, color, RotateX(PI) * rotation);
}

void DrawPlaneXZ(Vector3 center, float width, float depth, Vector3 color, Matrix rotation)
{
	Mesh mesh;
	GenPlaneXZ(mesh);

	Matrix world =
		Scale(width, 1.0f, depth) *
		rotation *
		Translate(center.x, center.y, center.z);

	Matrix mvp = world * gView * gProj;
	DrawMeshDebug(mesh, mvp, world, color);
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

	Matrix mvp = world * gView * gProj;
	DrawMeshDebug(mesh, mvp, world, color);
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

	Matrix mvp = world * gView * gProj;
	DrawMeshDebug(mesh, mvp, world, color);
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

	Matrix mvp = world * gView * gProj;
	DrawMeshDebug(mesh, mvp, world, color);
	DestroyMesh(mesh);
}

void DrawMeshDebug(Mesh mesh, Matrix mvp, Matrix world, Vector3 color)
{
	switch (gDebugShader)
	{
	case FLAT:
		DrawMeshFlat(mesh, mvp, color);
		break;

	case WIRE:
		DrawMeshWireframes(mesh, mvp, color);
		break;

	case POSITIONS_WORLD:
		DrawMeshPositionsWorld(mesh, mvp, world);
		break;

	case POSITIONS_SCREEN:
		DrawMeshPositionsScreen(mesh, mvp);
		break;

	case NORMALS_OBJECT:
		DrawMeshNormals(mesh, mvp, MatrixIdentity());
		break;

	case NORMALS_WORLD:
		DrawMeshNormals(mesh, mvp, world);
		break;

	case TCOORDS:
		DrawMeshTcoords(mesh, mvp);
		break;

	default:
		assert(false, "Invalid Debug Shader");
		break;
	}
}

void DrawMesh(Mesh mesh)
{
	glBindVertexArray(mesh.vao);
	glDrawArrays(GL_TRIANGLES, 0, mesh.vertexCount);
	glBindVertexArray(GL_NONE);
}

void DrawLine(Vector3 p0, Vector3 p1, Vector3 color, float thickness)
{
	bool depthTest = DepthTest();
	bool depthWrite = DepthWrite();
	SetDepthTest(false);
	SetDepthWrite(false);

	Matrix mvp = gView * gProj;
	BindShader(&gShaderLine);
	SendMat4("u_mvp", &mvp);
	SendVec3("u_pos0", p0);
	SendVec3("u_pos1", p1);
	SendVec3("u_color", color);
	BindEmptyVao();
	glLineWidth(thickness);
	glDrawArrays(GL_LINES, 0, 2);
	glLineWidth(1.0f);
	BindNullVao();
	UnbindShader();

	SetDepthWrite(depthWrite);
	SetDepthTest(depthTest);
}

void DrawFsq()
{
	bool depthTest = DepthTest();
	bool depthWrite = DepthWrite();
	SetDepthTest(false);
	SetDepthWrite(false);

	BindEmptyVao();
	glDrawArrays(GL_TRIANGLES, 0, 3);
	BindNullVao();

	SetDepthWrite(depthWrite);
	SetDepthTest(depthTest);
}

void DrawFsqTexture(Texture texture)
{
	BindTexture(texture);
	BindShader(&gShaderFSQ);
	SendInt("u_tex", 0);
	BindEmptyVao();
	DrawFsq();
	UnbindShader();
	UnbindTexture(texture);
}
