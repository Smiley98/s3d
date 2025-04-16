#include "Render.h"
#include <cassert>

static GLuint fVaoEmpty;

DebugShaderType gDebugShader = FLAT;
Matrix gView = MatrixIdentity();
Matrix gProj = MatrixIdentity();

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
	SetPipelineState(gPipelineWireframes);
	DrawMeshFlat(mesh, world, color);
	SetPipelineState(gPipelineDefault);
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

void DrawMeshReflect(const Mesh& mesh, Matrix world, TextureCubemap cubemap, GLuint unit)
{
	Matrix mvp = world * gView * gProj;
	BindTextureCubemap(cubemap, unit);
	BindShader(&gShaderEnvironmentReflect);
	SendMat4("u_mvp", mvp);
	SendMat4("u_world", world);
	SendMat3("u_normal", NormalMatrix(world));
	SendVec3("u_camPos", gCamera.position);
	DrawMesh(mesh);
	UnbindShader();
	UnbindTextureCubemap(cubemap, unit);
}

void DrawMeshRefract(const Mesh& mesh, Matrix world, TextureCubemap cubemap, GLuint unit, float ratio)
{
	Matrix mvp = world * gView * gProj;
	BindTextureCubemap(cubemap, unit);
	BindShader(&gShaderEnvironmentRefract);
	SendMat4("u_mvp", mvp);
	SendMat4("u_world", world);
	SendMat3("u_normal", NormalMatrix(world));
	SendVec3("u_camPos", gCamera.position);
	SendFloat("u_ratio", ratio);
	DrawMesh(mesh);
	UnbindShader();
	UnbindTextureCubemap(cubemap, unit);
}

void DrawTriangle(Vector2 v0, Vector2 v1, Vector2 v2, Vector3 color, float angle)
{
	Mesh mesh;
	GenMeshTriangle(&mesh, v0, v1, v2);
	CreateMesh(&mesh);
	DrawMeshDebug(mesh, RotateZ(angle), color);
	DestroyMesh(&mesh);
}

void DrawRectangle(Vector2 center, float width, float height, Vector3 color, float angle)
{
	Matrix world = Scale(width, height, 1.0f) * RotateZ(angle) * Translate(center.x, center.y, 0.0f);
	DrawMeshDebug(gMeshPlane, world, color);
}

void DrawCircle(Vector2 center, float radius, Vector3 color, float angle)
{
	Matrix world = Scale(radius, radius, 1.0f) * RotateZ(angle) *  Translate(center.x, center.y, 0.0f);
	DrawMeshDebug(gMeshCircle, world, color);
}

void DrawSemicircle(Vector2 center, float radius, Vector3 color, float angle)
{
	Matrix world = Scale(radius, radius, 1.0f) * RotateZ(angle) * Translate(center.x, center.y, 0.0f);
	DrawMeshDebug(gMeshSemicircle, world, color);
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
	DrawMeshDebug(gMeshCube, world, color);
}

void DrawSphere(Vector3 center, float radius, Vector3 color, Matrix rotation)
{
	Matrix world = Scale(radius, radius, radius) * rotation *  Translate(center.x, center.y, center.z);
	DrawMeshDebug(gMeshSphere, world, color);
}

void DrawHemisphere(Vector3 center, float radius, Vector3 color, Matrix rotation)
{
	Matrix world = Scale(radius, radius, radius) * rotation * Translate(center.x, center.y, center.z);
	DrawMeshDebug(gMeshHemisphere, world, color);
}

void DrawCylinder(Vector3 center, float radius, float halfHeight, Vector3 color, Matrix rotation)
{
	Matrix world = Scale(radius, radius, halfHeight * 2.0f) * rotation * Translate(center.x, center.y, center.z);
	DrawMeshDebug(gMeshCylinder, world, color);
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
	SetPipelineState(gPipelineNoDepth);
	Matrix mvp = gView * gProj;
	BindShader(&gShaderLine);
	SendMat4("u_mvp", mvp);
	SendVec3("u_pos0", p0);
	SendVec3("u_pos1", p1);
	SendVec3("u_color", color);

	glBindVertexArray(fVaoEmpty);
	glLineWidth(thickness);
	glDrawArrays(GL_LINES, 0, 2);
	glLineWidth(1.0f);
	glBindVertexArray(GL_NONE);

	UnbindShader();
	SetPipelineState(gPipelineDefault);
}

void DrawFsq(PipelineState* state)
{
	SetPipelineState(state != nullptr ? *state : gPipelineNoDepth);
	glBindVertexArray(fVaoEmpty);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(GL_NONE);
	SetPipelineState(gPipelineDefault);
}

void DrawFsqTexture(Texture2D texture, GLuint unit, PipelineState* state)
{
	BindTexture2D(texture, unit);
	BindShader(&gShaderFsq);
	SendInt("u_tex", unit);
	DrawFsq(state);
	UnbindShader();
	UnbindTexture2D(texture, unit);
}

void DrawColor(Framebuffer framebuffer, GLuint attachment, GLuint unit)
{
	DrawFsqTexture(*framebuffer.colors[attachment], unit);
}

void DrawDepth(Framebuffer framebuffer, GLuint unit)
{
	float near = gProj.m14 / (gProj.m10 - 1.0f);
	float far = gProj.m14 / (gProj.m10 + 1.0f);
	BindTexture2D(*framebuffer.depth, unit);
	BindShader(&gShaderFsqDepth);
	SendFloat("u_near", near);
	SendFloat("u_far", far);
	SendInt("u_tex", unit);
	DrawFsq();
	UnbindShader();
	UnbindTexture2D(*framebuffer.depth, unit);
}

void DrawSkybox(TextureCubemap cubemap, GLuint unit)
{
	Matrix viewSky = gView;
	viewSky.m12 = viewSky.m13 = viewSky.m14 = 0.0f;

	PipelineState sky = gPipelineDefault;
	sky.depthTest = true;
	sky.depthWrite = false;
	sky.cullFace = GL_FRONT;
	SetPipelineState(sky);

	BindTextureCubemap(cubemap, unit);
	BindShader(&gShaderSkybox);
	SendMat4("u_mvp", viewSky * gProj);
	DrawMesh(gMeshCube);
	UnbindShader();
	UnbindTextureCubemap(cubemap, unit);
	SetPipelineState(gPipelineDefault);
}

void InitRenderer()
{
	glGenVertexArrays(1, &fVaoEmpty);
}

void QuitRenderer()
{
	glDeleteVertexArrays(1, &fVaoEmpty);
	fVaoEmpty = GL_NONE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////CPU RENDERING///////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "Image.h"
Image gImageCPU;
static Texture2D fTextureCPU;

void InitSoftwareRenderer()
{
	CreateImageFromMemory(&gImageCPU, CPU_IMAGE_SIZE, CPU_IMAGE_SIZE);
	CreateTexture2D(&fTextureCPU, CPU_IMAGE_SIZE, CPU_IMAGE_SIZE, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, GL_NEAREST);
}

void QuitSoftwareRenderer()
{
	DestroyTexture2D(&fTextureCPU);
	DestroyImage(&gImageCPU);
}

void PresentSoftwareImage()
{
	BindTexture2D(fTextureCPU, 0);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, CPU_IMAGE_SIZE, CPU_IMAGE_SIZE, GL_RGBA, GL_UNSIGNED_BYTE, gImageCPU.pixels.data());
	UnbindTexture2D(fTextureCPU, 0);
	DrawFsqTexture(fTextureCPU, 0);
}
