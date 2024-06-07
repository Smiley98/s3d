#pragma once
#include <glad/glad.h>
#include <cstdint>
#include "Math.h"

// Could be generalized into a VertexBuffer struct.
struct Mesh
{
	// If we don't use stl, then arrays aren't visible in the debugger :(
	Vector3* positions = nullptr;
	Vector3* normals = nullptr;
	Vector2* tcoords = nullptr;
	Vector3* colors = nullptr;
	uint16_t* indices = nullptr;

	size_t vertexCount = 0;
	size_t faceCount = 0;

	// No need in CPU renderer, but easier to store all in one place!
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

// Meshes can be created either from obj, par, or manually,
// so it doesn't make sense to build an API when creation methods varry.
//void CreateMesh(Mesh* mesh, const char* path);
//void DestroyMesh(Mesh* mesh);

// Create more shapes with par_shapes!
void CreateMeshes();
void DestroyMeshes();

void BindFsq();
void DrawFsq();

void DrawMesh(Mesh mesh);
//void DrawMeshIndexed(const Mesh& mesh);