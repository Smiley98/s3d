#pragma once
#include "Config.h"
#include "Mesh.h"
#include "Mesh2.h"
#include "Shader.h"
#include "Texture.h"
#include "PipelineState.h"

enum DebugShaderType : int
{
	FLAT,
	WIRE,
	POSITIONS_WORLD,
	POSITIONS_SCREEN,
	NORMALS_OBJECT,
	NORMALS_WORLD,
	TCOORDS
};

extern DebugShaderType gDebugShader;
extern Matrix gView;
extern Matrix gProj;

void DrawMeshFlat(Mesh mesh, Matrix mvp, Vector3 color);
void DrawMeshWireframes(Mesh mesh, Matrix mvp, Vector3 color);
void DrawMeshPositionsWorld(Mesh mesh, Matrix mvp, Matrix world);
void DrawMeshPositionsScreen(Mesh mesh, Matrix mvp, Vector2 resolution = { SCREEN_WIDTH, SCREEN_HEIGHT });
void DrawMeshNormals(Mesh mesh, Matrix mvp, Matrix world/*identity for object-space*/);
void DrawMeshTcoords(Mesh mesh, Matrix mvp/*object-space*/);
void DrawMeshTexture(Mesh mesh, Matrix mvp, Matrix world, Texture texture);
//void DrawMeshTextureTint(Mesh mesh, Matrix mvp, Matrix world, Texture texture, Vector3 color);
// This needs a new shader and isn't immediately useful.

// 2d:
void DrawTriangle(Vector2 v0, Vector2 v1, Vector2 v2, Vector3 color, float angle = 0.0f);
void DrawCircle(Vector2 center, float radius, Vector3 color, float angle = 0.0f);
void DrawRectangle(Vector2 center, float width, float height, Vector3 color, float angle = 0.0f);
void DrawSemicircleH(Vector2 center, float radius, Vector3 color, float angle = 0.0f);
void DrawSemicircleV(Vector2 center, float radius, Vector3 color, float angle = 0.0f);
void DrawCapsuleH(Vector2 center, float radius, float halfHeight, Vector3 color, float angle = 0.0f);
void DrawCapsuleV(Vector2 center, float radius, float halfHeight, Vector3 color, float angle = 0.0f);

// 3d:
void DrawCube(Vector3 center, float width, float height, float depth, Vector3 color, Matrix rotation = MatrixIdentity());
void DrawSphere(Vector3 center, float radius, Vector3 color, Matrix rotation = MatrixIdentity());
void DrawHemisphere(Vector3 center, float radius, Vector3 color, Matrix rotation = MatrixIdentity());
void DrawCylinder(Vector3 center, float radius, float halfHeight, Vector3 color, Matrix rotation = MatrixIdentity());
void DrawSpherocylinder(Vector3 center, float radius, float halfHeight, Vector3 color, Matrix rotation = MatrixIdentity());

void DrawPlaneXZ(Vector3 center, float width, float depth, Vector3 color, Matrix rotation = MatrixIdentity());
void DrawPlaneYZ(Vector3 center, float height, float depth, Vector3 color, Matrix rotation = MatrixIdentity());
void DrawPlaneXY(Vector3 center, float width, float height, Vector3 color, Matrix rotation = MatrixIdentity());
void DrawDodecahedron(Vector3 center, Vector3 scale, Vector3 color, Matrix rotation = MatrixIdentity());

// Renders mesh based on gDebugShader
void DrawMeshDebug(Mesh mesh, Matrix mvp, Matrix world, Vector3 color);

// Renders mesh as triangles based on bound pipeline state
void DrawMesh(Mesh mesh);

// Renders mesh as triangles using indexed rendering
void DrawMesh2(Mesh2 mesh);

void DrawMesh2Flat(Mesh2 mesh, Matrix world, Vector3 color = V3_ONE);
void DrawMesh2Normals(Mesh2 mesh, Matrix world);
void DrawMesh2Tcoords(Mesh2 mesh, Matrix world);


// Renders a full-screen quad (no depth test or depth write)
void DrawFsq();
void DrawFsqTexture(Texture texture);

// Renders a line (no depth test or depth write)
void DrawLine(Vector3 p0, Vector3 p1, Vector3 color, float thickness = 1.0f);
