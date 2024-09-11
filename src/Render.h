#pragma once
#include "Config.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"

void SetView(Matrix view);
void SetProj(Matrix proj);
void SetWireframes(bool wireframes);

void DrawMeshFlat(Mesh mesh, Matrix mvp, Vector3 color);
void DrawMeshWireframes(Mesh mesh, Matrix mvp, Vector3 color);
void DrawMeshNormals(Mesh mesh, Matrix mvp, Matrix world/*identity for object-space*/);
void DrawMeshPositionsWorld(Mesh mesh, Matrix mvp, Matrix world);
void DrawMeshPositionsScreen(Mesh mesh, Matrix mvp, Vector2 resolution = { SCREEN_WIDTH, SCREEN_HEIGHT });

// 2d:
void DrawTriangle(Vector2 v0, Vector2 v1, Vector2 v2, Vector3 color);
void DrawCircle(Vector2 center, float radius, Vector3 color);
void DrawRectangle(Vector2 center, float width, float height, Vector3 color, float angle = 0.0f);
void DrawSemicircleH(Vector2 center, float radius, Vector3 color, float angle = 0.0f);
void DrawSemicircleV(Vector2 center, float radius, Vector3 color, float angle = 0.0f);
void DrawCapsuleH(Vector2 center, float radius, float halfHeight, Vector3 color, float angle = 0.0f);
void DrawCapsuleV(Vector2 center, float radius, float halfHeight, Vector3 color, float angle = 0.0f);
//#define DrawCapsule DrawCapsuleH <-- stops mouse-over definition preview :(

// 3d:
void DrawCube(Vector3 center, float width, float height, float depth, Vector3 color, Matrix rotation = MatrixIdentity());
void DrawSphere(Vector3 center, float radius, Vector3 color, Matrix rotation = MatrixIdentity());
//void DrawHemisphere();
//void DrawSpherocylinder(); // <-- "capsule" xD
