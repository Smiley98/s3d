#pragma once
#include "Config.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"
#include "Framebuffer.h"
#include "PipelineState.h"

extern Matrix gView;
extern Matrix gProj;

enum DebugShaderType : int
{
	FLAT,
	WIRE,
	DEPTH,
	POSITIONS_WORLD,
	POSITIONS_SCREEN,
	NORMALS_OBJECT,
	NORMALS_WORLD,
	TCOORDS
};

// Shader testing
extern DebugShaderType gDebugShader;
void DrawMeshDebug(const Mesh& mesh, Matrix world, Vector3 color);

void DrawMeshFlat(const Mesh& mesh, Matrix world, Vector3 color);
void DrawMeshWireframes(const Mesh& mesh, Matrix world, Vector3 color);
void DrawMeshDepth(const Mesh& mesh, Matrix world);/*Assumes perspective projection*/
void DrawMeshPositionsWorld(const Mesh& mesh, Matrix world);
void DrawMeshPositionsScreen(const Mesh& mesh, Matrix world, Vector2 resolution = { SCREEN_WIDTH, SCREEN_HEIGHT });
void DrawMeshNormals(const Mesh& mesh, Matrix world, Matrix normal/*identity for object-space*/);
void DrawMeshTcoords(const Mesh& mesh, Matrix world/*object-space*/);

// Object effects
void DrawMeshTexture(const Mesh& mesh, Matrix world, Texture texture);
void DrawMeshReflect(const Mesh& mesh, Matrix world, Cubemap cubemap);
void DrawMeshRefract(const Mesh& mesh, Matrix world, Cubemap cubemap, float ratio);

// 2d:
void DrawTriangle(Vector2 v0, Vector2 v1, Vector2 v2, Vector3 color, float angle = 0.0f);
void DrawCircle(Vector2 center, float radius, Vector3 color, float angle = 0.0f);
void DrawRectangle(Vector2 center, float width, float height, Vector3 color, float angle = 0.0f);
void DrawSemicircleX(Vector2 center, float radius, Vector3 color, float angle = 0.0f);
void DrawSemicircleY(Vector2 center, float radius, Vector3 color, float angle = 0.0f);
void DrawCapsuleX(Vector2 center, float radius, float halfHeight, Vector3 color, float angle = 0.0f);
void DrawCapsuleY(Vector2 center, float radius, float halfHeight, Vector3 color, float angle = 0.0f);

// 3d:
void DrawLine(Vector3 p0, Vector3 p1, Vector3 color, float thickness = 1.0f);
void DrawCube(Vector3 center, float width, float height, float depth, Vector3 color, Matrix rotation = MatrixIdentity());
void DrawSphere(Vector3 center, float radius, Vector3 color, Matrix rotation = MatrixIdentity());
void DrawHemisphere(Vector3 center, float radius, Vector3 color, Matrix rotation = MatrixIdentity());
void DrawCylinder(Vector3 center, float radius, float halfHeight, Vector3 color, Matrix rotation = MatrixIdentity());
void DrawSpherocylinder(Vector3 center, float radius, float halfHeight, Vector3 color, Matrix rotation = MatrixIdentity());
void DrawPlaneZ(Vector3 center, float width, float height, Vector3 color, Matrix rotation = MatrixIdentity());
void DrawPlaneY(Vector3 center, float width, float depth, Vector3 color, Matrix rotation = MatrixIdentity());
void DrawPlaneX(Vector3 center, float height, float depth, Vector3 color, Matrix rotation = MatrixIdentity());

// Fullscreen quad
void DrawFsq();
void DrawFsqTexture(Texture texture, int slot = 0);

// Framebuffer
void DrawColor(Framebuffer framebuffer, int slot = 0);
void DrawDepth(Framebuffer framebuffer);

// Cubemap
void DrawSkybox(Cubemap cubemap);

// Mesh
void DrawMesh(const Mesh& mesh);
void DrawMeshInstanced(const Mesh& mesh, int instanceCount);
