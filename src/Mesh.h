#pragma once
#include <glad/glad.h>
#include <par_shapes.h>
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

// Generated meshes
extern Mesh gMeshTriangle;
extern Mesh gMeshCube;
extern Mesh gMeshSphere;
extern Mesh gMeshDodecahedron;

// Imported meshes
extern Mesh gMeshHead;

void CreateMeshes();
void DestroyMeshes();

// Create mesh from data
void CreateMesh(Mesh* mesh, size_t vc, Vector3* positions, Vector3* normals, Vector2* tcoords, Vector3* colors, uint16_t* indices);
void DestroyMesh(Mesh* mesh);

// Create mesh from obj file
void CreateMeshObj(Mesh* mesh, const char* file);

// Create mesh from par_shapes
void CreateMeshPar(Mesh* mesh, par_shapes_mesh* par_mesh);

void CopyMesh(Mesh src, Mesh* dst);
void DrawMesh(Mesh mesh);

void BindFsq();
void DrawFsq();
