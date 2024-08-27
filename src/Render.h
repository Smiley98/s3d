#pragma once
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"

void DrawMeshFlat(Mesh mesh, Matrix mvp, Vector3 color);
void DrawMeshWireframes(Mesh mesh, Matrix mvp, Vector3 color);
void DrawMeshPositions(Mesh mesh, Matrix mvp, Matrix world);
void DrawMeshNormals(Mesh mesh, Matrix mvp, Matrix world/*identity for object-space*/);

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
