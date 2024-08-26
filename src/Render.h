// Not really sure about the structure.
// Just a placeholder for generic draw calls!
#pragma once
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"

void DrawMeshWireframes(Mesh mesh, Matrix mvp, Vector3 color);
void DrawMeshNormals(Mesh mesh, Matrix mvp, Matrix world = MatrixIdentity());

void DrawSphere(Vector3 position, Vector3 color);