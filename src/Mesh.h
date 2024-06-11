#pragma once
#include <glad/glad.h>
#include <cstdint>
#include <cassert>
#include "Math.h"

struct Mesh
{
	// Pros: no accidental copies. Cons: memory-corruption prone & invisible in the debugger.
	Vector3* positions = nullptr;
	Vector3* normals = nullptr;
	Vector2* tcoords = nullptr;
	Vector3* colors = nullptr;
	uint16_t* indices = nullptr;

	size_t vertexCount = 0;
	size_t faceCount = 0;

	// Easiest to store CPU & GPU data in the same structure.
	GLuint vao = GL_NONE;	// Vertex array object
	GLuint vbo = GL_NONE;	// Vertex positions
	GLuint nbo = GL_NONE;	// Vertex normals
	GLuint tbo = GL_NONE;	// Vertex texture coordinates
	GLuint cbo = GL_NONE;	// Vertex colors
	GLuint ibo = GL_NONE;	// Vertex indices
};

extern Mesh gMeshTriangle;
extern Mesh gMeshCube;
extern Mesh gMeshSphere;
extern Mesh gMeshHead;
extern Mesh gMeshDodecahedron;

void CreateMeshes();
void DestroyMeshes();

void BindFsq();
void DrawFsq();

void DrawMesh(Mesh mesh);
