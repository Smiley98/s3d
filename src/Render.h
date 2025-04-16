#pragma once
#include "Config.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"
#include "Framebuffer.h"
#include "PipelineState.h"
// ***Draw calls from Render.h always revert back to default pipeline state***

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

extern DebugShaderType gDebugShader;
void DrawMeshDebug(const Mesh& mesh, Matrix world, Vector3 color);

// Debug effects
void DrawMeshFlat(const Mesh& mesh, Matrix world, Vector3 color);
void DrawMeshWireframes(const Mesh& mesh, Matrix world, Vector3 color);
void DrawMeshDepth(const Mesh& mesh, Matrix world);/*Assumes perspective projection*/
void DrawMeshPositionsWorld(const Mesh& mesh, Matrix world);
void DrawMeshPositionsScreen(const Mesh& mesh, Matrix world, Vector2 resolution = { SCREEN_WIDTH, SCREEN_HEIGHT });
void DrawMeshNormals(const Mesh& mesh, Matrix world, Matrix normal/*identity for object-space*/);
void DrawMeshTcoords(const Mesh& mesh, Matrix world/*object-space*/);

// Textured effects
void DrawMeshTexture(const Mesh& mesh, Matrix world, Texture2D texture, GLuint unit);
void DrawMeshReflect(const Mesh& mesh, Matrix world, TextureCubemap cubemap, GLuint unit);
void DrawMeshRefract(const Mesh& mesh, Matrix world, TextureCubemap cubemap, GLuint unit, float ratio);

// 2D primitives
void DrawTriangle(Vector2 v0, Vector2 v1, Vector2 v2, Vector3 color, float angle = 0.0f);
void DrawRectangle(Vector2 center, float width, float height, Vector3 color, float angle = 0.0f);
void DrawCircle(Vector2 center, float radius, Vector3 color, float angle = 0.0f);
void DrawSemicircleX(Vector2 center, float radius, Vector3 color, float angle = 0.0f);
void DrawSemicircleY(Vector2 center, float radius, Vector3 color, float angle = 0.0f);
void DrawCapsuleX(Vector2 center, float radius, float halfHeight, Vector3 color, float angle = 0.0f);
void DrawCapsuleY(Vector2 center, float radius, float halfHeight, Vector3 color, float angle = 0.0f);

// 3D primitives
void DrawLine(Vector3 p0, Vector3 p1, Vector3 color, float thickness = 1.0f);
void DrawCube(Vector3 center, float width, float height, float depth, Vector3 color, Matrix rotation = MatrixIdentity());
void DrawSphere(Vector3 center, float radius, Vector3 color, Matrix rotation = MatrixIdentity());
void DrawHemisphere(Vector3 center, float radius, Vector3 color, Matrix rotation = MatrixIdentity());
void DrawCylinder(Vector3 center, float radius, float halfHeight, Vector3 color, Matrix rotation = MatrixIdentity());
void DrawSpherocylinder(Vector3 center, float radius, float halfHeight, Vector3 color, Matrix rotation = MatrixIdentity());

// Fullscreen quad
void DrawFsq(PipelineState* state = nullptr/*Hybrid rendering needs depth*/);
void DrawFsqTexture(Texture2D texture, GLuint unit, PipelineState* state = nullptr);

// Framebuffer
void DrawColor(Framebuffer framebuffer, GLuint attachment, GLuint unit = 0);
void DrawDepth(Framebuffer framebuffer, GLuint unit = 0);

// Cubemap
void DrawSkybox(TextureCubemap cubemap, GLuint unit);

// Mesh
void DrawMesh(const Mesh& mesh);
void DrawMeshInstanced(const Mesh& mesh, int instanceCount);

void InitRenderer();
void QuitRenderer();

// CPU rendering
void InitSoftwareRenderer();
void QuitSoftwareRenderer();

void PresentSoftwareImage();
