#include "Render.h"
#include <cassert>

DebugShaderType gDebugShader = FLAT;
Matrix gView = MatrixIdentity();
Matrix gProj = MatrixIdentity();

Mesh gMeshCircle;
Mesh gMeshSphere;
Mesh gMeshCube;

struct Shapes
{
	// 2D
	Mesh square;
	Mesh circle;
	Mesh semicircle;

	// 3D
	Mesh cube;
	Mesh sphere;
	Mesh hemisphere;
	Mesh cylinder;
} fShapes;

void SetDebugShader(DebugShaderType type)
{
	gDebugShader = type;
}

void DrawMeshFlat(const Mesh& mesh, Matrix world, Vector3 color)
{
	Matrix mvp = world * gView * gProj;
	BindShader(&gShaderColor);
	SendMat4("u_mvp", mvp);
	SendVec3("u_color", color);
	DrawMesh(mesh);
	UnbindShader();
}

void DrawMeshWireframes(const Mesh& mesh, Matrix world, Vector3 color)
{
	bool depthTest = DepthTest();
	bool depthWrite = DepthWrite();
	SetDepthTest(false);
	SetDepthWrite(false);
	SetWireframes(true);
	DrawMeshFlat(mesh, world, color);
	SetWireframes(false);
	SetDepthWrite(depthTest);
	SetDepthTest(depthWrite);
}

// Assumes perspective projection.
// (Could make a system that stores all perspective & ortho values upon set, but I don't think we need that right now).
void DrawMeshDepth(const Mesh& mesh, Matrix world)
{
	float c = gProj.m10;
	float d = gProj.m14;
	float near = d / (c - 1.0f);
	float far = d / (c + 1.0f);
	Matrix mvp = world * gView * gProj;
	BindShader(&gShaderDepth);
	SendMat4("u_mvp", mvp);
	SendMat4("u_world", world);
	SendFloat("u_near", near);
	SendFloat("u_far", far);
	DrawMesh(mesh);
	UnbindShader();
}

void DrawMeshPositionsWorld(const Mesh& mesh, Matrix world)
{
	Matrix mvp = world * gView * gProj;
	BindShader(&gShaderPositionsWorld);
	SendMat4("u_mvp", mvp);
	SendMat4("u_world", world);
	DrawMesh(mesh);
	UnbindShader();
}

void DrawMeshPositionsScreen(const Mesh& mesh, Matrix world, Vector2 resolution)
{
	Matrix mvp = world * gView * gProj;
	BindShader(&gShaderPositionsScreen);
	SendMat4("u_mvp", mvp);
	SendVec2("u_resolution", resolution);
	DrawMesh(mesh);
	UnbindShader();
}

void DrawMeshNormals(const Mesh& mesh, Matrix world, Matrix normal)
{
	Matrix mvp = world * gView * gProj;
	BindShader(&gShaderNormals);
	SendMat4("u_mvp", mvp);
	SendMat4("u_world", world);
	SendMat3("u_normal", normal);
	DrawMesh(mesh);
	UnbindShader();
}

void DrawMeshTcoords(const Mesh& mesh, Matrix world)
{
	if (!mesh.tcoords.empty())
	{
		Matrix mvp = world * gView * gProj;
		BindShader(&gShaderTcoords);
		SendMat4("u_mvp", mvp);
		DrawMesh(mesh);
		UnbindShader();
	}
}

void DrawMeshTexture(const Mesh& mesh, Matrix world, Texture2D texture, GLuint unit)
{
	assert(!mesh.tcoords.empty());
	Matrix normal = NormalMatrix(world);
	Matrix mvp = world * gView * gProj;
	BindTexture2D(texture, unit);
	BindShader(&gShaderTexture);
	SendMat4("u_mvp", mvp);
	SendMat3("u_normal", normal);
	SendInt("u_tex", unit);
	DrawMesh(mesh);
	UnbindShader();
	UnbindTexture2D(texture, unit);
}

void DrawMeshReflect(const Mesh& mesh, Matrix world, Cubemap cubemap, GLuint unit)
{
	Matrix mvp = world * gView * gProj;
	BindCubemap(cubemap, unit);
	BindShader(&gShaderEnvironmentReflect);
	SendMat4("u_mvp", mvp);
	SendMat4("u_world", world);
	SendMat3("u_normal", NormalMatrix(world));
	SendVec3("u_camPos", gCamera.position);
	DrawMesh(mesh);
	UnbindShader();
	UnbindCubemap(cubemap, unit);
}

void DrawMeshRefract(const Mesh& mesh, Matrix world, Cubemap cubemap, GLuint unit, float ratio)
{
	Matrix mvp = world * gView * gProj;
	BindCubemap(cubemap, unit);
	BindShader(&gShaderEnvironmentRefract);
	SendMat4("u_mvp", mvp);
	SendMat4("u_world", world);
	SendMat3("u_normal", NormalMatrix(world));
	SendVec3("u_camPos", gCamera.position);
	SendFloat("u_ratio", ratio);
	DrawMesh(mesh);
	UnbindShader();
	UnbindCubemap(cubemap, unit);
}

void DrawRectangle(Vector2 center, float width, float height, Vector3 color, float angle)
{
	Matrix world = Scale(width, height, 1.0f) * RotateZ(angle) * Translate(center.x, center.y, 0.0f);
	DrawMeshDebug(fShapes.square, world, color);
}

void DrawTriangle(Vector2 v0, Vector2 v1, Vector2 v2, Vector3 color, float angle)
{
	Mesh mesh;
	GenMeshTriangle(&mesh, v0, v1, v2);
	CreateMesh(&mesh);
	DrawMeshDebug(mesh, RotateZ(angle), color);
	DestroyMesh(&mesh);
}

void DrawCircle(Vector2 center, float radius, Vector3 color, float angle)
{
	Matrix world = Scale(radius, radius, 1.0f) * RotateZ(angle) *  Translate(center.x, center.y, 0.0f);
	DrawMeshDebug(fShapes.circle, world, color);
}

void DrawSemicircle(Vector2 center, float radius, Vector3 color, float angle)
{
	Matrix world = Scale(radius, radius, 1.0f) * RotateZ(angle) * Translate(center.x, center.y, 0.0f);
	DrawMeshDebug(fShapes.semicircle, world, color);
}

void DrawSemicircleX(Vector2 center, float radius, Vector3 color, float angle)
{
	DrawSemicircle(center, radius, color, angle - PI * 0.5f);
}

void DrawSemicircleY(Vector2 center, float radius, Vector3 color, float angle)
{
	DrawSemicircle(center, radius, color, angle);
}

void DrawCapsuleX(Vector2 center, float radius, float halfHeight, Vector3 color, float angle)
{
	Vector2 direction = Rotate(V2_RIGHT, angle);
	Vector2 top = center + direction * halfHeight;
	Vector2 bot = center - direction * halfHeight;
	DrawSemicircleX(top, radius, color, angle);
	DrawSemicircleX(bot, radius, color, angle + PI);
	DrawRectangle(center, halfHeight * 2.0f, radius * 2.0f, color, angle);
}

void DrawCapsuleY(Vector2 center, float radius, float halfHeight, Vector3 color, float angle)
{
	Vector2 direction = Rotate(V2_UP, angle);
	Vector2 top = center + direction * halfHeight;
	Vector2 bot = center - direction * halfHeight;
	DrawSemicircleY(top, radius, color, angle);
	DrawSemicircleY(bot, radius, color, angle + PI);
	DrawRectangle(center, radius * 2.0f, halfHeight * 2.0f, color, angle);
}

void DrawCube(Vector3 center, float width, float height, float depth, Vector3 color, Matrix rotation)
{
	Matrix world = Scale(width, height, depth) * rotation * Translate(center.x, center.y, center.z);
	DrawMeshDebug(fShapes.cube, world, color);
}

void DrawSphere(Vector3 center, float radius, Vector3 color, Matrix rotation)
{
	Matrix world = Scale(radius, radius, radius) * rotation *  Translate(center.x, center.y, center.z);
	DrawMeshDebug(fShapes.sphere, world, color);
}

void DrawHemisphere(Vector3 center, float radius, Vector3 color, Matrix rotation)
{
	Matrix world = Scale(radius, radius, radius) * rotation * Translate(center.x, center.y, center.z);
	DrawMeshDebug(fShapes.hemisphere, world, color);
}

void DrawCylinder(Vector3 center, float radius, float halfHeight, Vector3 color, Matrix rotation)
{
	Matrix world = Scale(radius, radius, halfHeight * 2.0f) * rotation * Translate(center.x, center.y, center.z);
	DrawMeshDebug(fShapes.cylinder, world, color);
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

void DrawMeshDebug(const Mesh& mesh, Matrix world, Vector3 color)
{
	switch (gDebugShader)
	{
	case FLAT:
		DrawMeshFlat(mesh, world, color);
		break;

	case WIRE:
		DrawMeshWireframes(mesh, world, color);
		break;

	case DEPTH:
		DrawMeshDepth(mesh, world);
		break;

	case POSITIONS_WORLD:
		DrawMeshPositionsWorld(mesh, world);
		break;

	case POSITIONS_SCREEN:
		DrawMeshPositionsScreen(mesh, world);
		break;

	case NORMALS_OBJECT:
		DrawMeshNormals(mesh, world, MatrixIdentity());
		break;

	case NORMALS_WORLD:
		DrawMeshNormals(mesh, world, NormalMatrix(world));
		break;

	case TCOORDS:
		DrawMeshTcoords(mesh, world);
		break;

	default:
		assert(false, "Invalid Debug Shader");
		break;
	}
}

void DrawMesh(const Mesh& mesh)
{
	glBindVertexArray(mesh.vao);
	if (mesh.ebo != GL_NONE)
	{
		glDrawElements(GL_TRIANGLES, mesh.count, GL_UNSIGNED_SHORT, nullptr);
	}
	else
	{
		glDrawArrays(GL_TRIANGLES, 0, mesh.count);
	}
	glBindVertexArray(GL_NONE);
}

void DrawMeshInstanced(const Mesh& mesh, int instanceCount)
{
	glBindVertexArray(mesh.vao);
	if (mesh.ebo != GL_NONE)
	{
		glDrawElementsInstanced(GL_TRIANGLES, mesh.count, GL_UNSIGNED_SHORT, nullptr, instanceCount);
	}
	else
	{
		glDrawArraysInstanced(GL_TRIANGLES, 0, mesh.count, instanceCount);
	}
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
	SendMat4("u_mvp", mvp);
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

void DrawFsqTexture(Texture2D texture, GLuint unit)
{
	BindTexture2D(texture, unit);
	BindShader(&gShaderFsq);
	SendInt("u_tex", unit);
	BindEmptyVao();
	DrawFsq();
	UnbindShader();
	UnbindTexture2D(texture, unit);
}

void DrawColor(Framebuffer framebuffer, GLuint attachment)
{
	// Defaulting to texture unit 0 is now error-proof
	DrawFsqTexture(framebuffer.colors[attachment], 0);
}

void DrawDepth(Framebuffer framebuffer)
{
	float near = gProj.m14 / (gProj.m10 - 1.0f);
	float far = gProj.m14 / (gProj.m10 + 1.0f);
	BindTexture2D(framebuffer.depth, 0);
	BindShader(&gShaderFsqDepth);
	SendFloat("u_near", near);
	SendFloat("u_far", far);
	SendInt("u_tex", 0);
	BindEmptyVao();
	DrawFsq();
	UnbindShader();
	UnbindTexture2D(framebuffer.depth, 0);
}

void DrawSkybox(Cubemap cubemap, GLuint unit)
{
	Matrix viewSky = gView;
	viewSky.m12 = viewSky.m13 = viewSky.m14 = 0.0f;

	bool depthTest = DepthTest();
	bool depthWrite = DepthWrite();
	GLenum faceCulling = FaceCulling();
	SetDepthTest(true);
	SetDepthWrite(false);
	SetFaceCulling(GL_FRONT);

	BindCubemap(cubemap, unit);
	BindShader(&gShaderSkybox);
	SendMat4("u_mvp", viewSky * gProj);
	DrawMesh(fShapes.cube);
	UnbindShader();
	UnbindCubemap(cubemap, unit);

	SetFaceCulling(faceCulling);
	SetDepthWrite(depthWrite);
	SetDepthTest(depthTest);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////CPU RENDERING///////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "Image.h"
static Texture2D fSoftwareRenderTexture;

void Present(Image* image)
{
	assert(!image->pixels.empty() && image->width == CPU_IMAGE_SIZE && image->height == CPU_IMAGE_SIZE);

	BindTexture2D(fSoftwareRenderTexture, 0);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, CPU_IMAGE_SIZE, CPU_IMAGE_SIZE, GL_RGBA, GL_UNSIGNED_BYTE, image->pixels.data());
	UnbindTexture2D(fSoftwareRenderTexture, 0);

	DrawFsqTexture(fSoftwareRenderTexture, 0);
}

void InitDebugRenderer()
{
	GenMeshPlane(&fShapes.square);
	GenMeshCircle(&fShapes.circle);
	GenMeshSemicircle(&fShapes.semicircle);

	GenMeshCube(&fShapes.cube);
	GenMeshSphere(&fShapes.sphere);
	GenMeshHemisphere(&fShapes.hemisphere);
	GenMeshCylinder(&fShapes.cylinder);

	CopyMesh(&gMeshCircle, &fShapes.circle);
	CopyMesh(&gMeshSphere, &fShapes.sphere);
	CopyMesh(&gMeshCube, &fShapes.cube);

	CreateMesh(&fShapes.square);
	CreateMesh(&fShapes.circle);
	CreateMesh(&fShapes.semicircle);

	CreateMesh(&fShapes.cube);
	CreateMesh(&fShapes.sphere);
	CreateMesh(&fShapes.hemisphere);
	CreateMesh(&fShapes.cylinder);

	CreateMesh(&gMeshCircle);
	CreateMesh(&gMeshSphere);
	CreateMesh(&gMeshCube);
}

void QuitDebugRenderer()
{
	DestroyMesh(&gMeshCube);
	DestroyMesh(&gMeshSphere);
	DestroyMesh(&gMeshCircle);

	DestroyMesh(&fShapes.cylinder);
	DestroyMesh(&fShapes.hemisphere);
	DestroyMesh(&fShapes.sphere);
	DestroyMesh(&fShapes.cube);
	
	DestroyMesh(&fShapes.semicircle);
	DestroyMesh(&fShapes.circle);
	DestroyMesh(&fShapes.square);
}

void InitSoftwareRenderer()
{
	CreateTexture2D(&fSoftwareRenderTexture, CPU_IMAGE_SIZE, CPU_IMAGE_SIZE, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, GL_NEAREST);
}

void QuitSoftwareRenderer()
{
	DestroyTexture2D(&fSoftwareRenderTexture);
}
