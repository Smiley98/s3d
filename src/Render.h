#pragma once
#include "Config.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"

void SetWireframes(bool wireframes);

void DrawMeshFlat(Mesh mesh, Matrix mvp, Vector3 color);
void DrawMeshWireframes(Mesh mesh, Matrix mvp, Vector3 color);
void DrawMeshNormals(Mesh mesh, Matrix mvp, Matrix world/*identity for object-space*/);
void DrawMeshPositionsWorld(Mesh mesh, Matrix mvp, Matrix world);
void DrawMeshPositionsScreen(Mesh mesh, Matrix mvp, Vector2 resolution = { SCREEN_WIDTH, SCREEN_HEIGHT });

// 2d:
//void DrawCircle();
//void DrawRec();
//void DrawSemicircle();
//void DrawCapsule();

// 3d:
//void DrawSphere(Vector3 position, float radius, Vector3 color);
//void DrawBox();
//void DrawHemisphere();
//void DrawSpherocylinder(); // <-- "capsule" xD
