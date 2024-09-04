#pragma once
#include "Config.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"

extern Matrix gView;
extern Matrix gProj;

void SetWireframes(bool wireframes);

void DrawMeshFlat(Mesh mesh, Matrix mvp, Vector3 color);
void DrawMeshWireframes(Mesh mesh, Matrix mvp, Vector3 color);
void DrawMeshNormals(Mesh mesh, Matrix mvp, Matrix world/*identity for object-space*/);
void DrawMeshPositionsWorld(Mesh mesh, Matrix mvp, Matrix world);
void DrawMeshPositionsScreen(Mesh mesh, Matrix mvp, Vector2 resolution = { SCREEN_WIDTH, SCREEN_HEIGHT });

// 2d:
void DrawRectangle(Vector2 center, float width, float height, Vector3 color, float angle = 0.0f);
void DrawCircle(Vector2 center, float radius, Vector3 color);
void DrawSemicircle(Vector2 center, float radius, Vector3 color, float angle = 0.0f);
void DrawCapsule(Vector2 center, float radius, float halfHeight, Vector3 color, float angle = 0.0f);

// 3d:
//void DrawBox();
//void DrawSphere(Vector3 position, float radius, Vector3 color);
//void DrawHemisphere();
//void DrawSpherocylinder(); // <-- "capsule" xD
