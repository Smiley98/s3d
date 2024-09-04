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
void DrawCircle(Vector2 center, float radius, Vector3 color);
void DrawRectangle(Vector2 center, float width, float height, Vector3 color, float angle = 0.0f);
void DrawSemicircleH(Vector2 center, float radius, Vector3 color, float angle = 0.0f);
void DrawSemicircleV(Vector2 center, float radius, Vector3 color, float angle = 0.0f);
void DrawCapsuleH(Vector2 center, float radius, float halfHeight, Vector3 color, float angle = 0.0f);
void DrawCapsuleV(Vector2 center, float radius, float halfHeight, Vector3 color, float angle = 0.0f);
//#define DrawCapsule DrawCapsuleH <-- stops mouse-over definition preview :(

// 3d:
//void DrawBox();
//void DrawSphere(Vector3 position, float radius, Vector3 color);
//void DrawHemisphere();
//void DrawSpherocylinder(); // <-- "capsule" xD
