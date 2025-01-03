#pragma once
#include <glad/glad.h>
#include <vector>
#include "Math.h"

enum MeshType
{
	MESH_TRIANGLE,
	MESH_SQUARE,
	MESH_CIRCLE,
	MESH_SEMICIRCLE,

	MESH_CUBE,
	MESH_SPHERE,
	MESH_HEMISPHERE,
	MESH_CYLINDER,

	MESH_PLANE_Z, // Faces +Z
	MESH_PLANE_Y, // Faces +Y
	MESH_PLANE_X, // Faces +X
};

struct Mesh
{
	int count = 0;
	std::vector<Vector3> positions;
	std::vector<Vector3> normals;
	std::vector<Vector2> tcoords;
	std::vector<uint16_t> indices;

	GLuint vao = GL_NONE;	// Vertex array object
	GLuint pbo = GL_NONE;	// Vertex positions
	GLuint nbo = GL_NONE;	// Vertex normals
	GLuint tbo = GL_NONE;	// Vertex texture coordinates
	GLuint ebo = GL_NONE;	// Vertex indices
};

extern Mesh gMeshCircle;// Unit circle r = 1
extern Mesh gMeshSphere;// Unit sphere r = 1
extern Mesh gMeshCube;	// Unit cube L x W x H = 1

extern Mesh gMeshHead;
extern Mesh gMeshTd;	// 10 x 10 x 50
extern Mesh gMeshGround;// 1x1 PLANE_XZ

void CreateMeshes();
void DestroyMeshes();

void CreateMesh(Mesh* mesh, const char* path, bool gpu = true);
void CreateMesh(Mesh* mesh, MeshType type, bool gpu = true);
void DestroyMesh(Mesh* mesh);

// par meshes:
// Optimized. Using par for parametric meshes, using custom algorithms for platonic solids.
// 
// obj meshes:
// Unoptimized. Not worth optimizing given the scope of this project.
// Potential optimizations include 1) generate index buffer and 2) load from binary.
