#pragma once
#include <glad/glad.h>
#include <vector>
#include "Math.h"

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

// 2D
extern Mesh gMeshPlane;
extern Mesh gMeshCircle;
extern Mesh gMeshSemicircle;

// 3D
extern Mesh gMeshCube;
extern Mesh gMeshSphere;
extern Mesh gMeshHemisphere;
extern Mesh gMeshCylinder;

// Game meshes
extern Mesh gMeshHead;
extern Mesh gMeshCt4;
extern Mesh gMeshGround;	// 1x1 PLANE_XZ (faces +Y)
extern Mesh gMeshHexagon;	// Unit hexagon r = 1, faces +Y
extern Mesh gMeshTd;		// 10 x 10 x 50

void CreateMeshes();	// Create game meshes
void DestroyMeshes();	// Destroy game meshes

void CreateMesh(Mesh* mesh);	// Allocate mesh on the GPU
void DestroyMesh(Mesh* mesh);	// Deallocate mesh on the GPU

void TransformMesh(Mesh* mesh, Matrix transform);	// CPU only
void CopyMesh(Mesh* dst, Mesh* src);				// CPU only

void GenMeshObj(Mesh* mesh, const char* path);

void GenMeshTriangle(Mesh* mesh, Vector3 v0, Vector3 v1, Vector3 v2);
void GenMeshPlane(Mesh* mesh, float width = 1.0f, float height = 1.0f, int divisions = 1);
void GenMeshCircle(Mesh* mesh, float radius = 1.0f, int divisions = 32);
void GenMeshSemicircle(Mesh* mesh, float radius = 1.0f, int divisions = 16);

void GenMeshCube(Mesh* mesh, float width = 1.0f, float height = 1.0f, float depth = 1.0f);
void GenMeshSphere(Mesh* mesh, float radius = 1.0f, int divisions = 8);
void GenMeshHemisphere(Mesh* mesh, float radius = 1.0f, int divisions = 4);
void GenMeshCylinder(Mesh* mesh, float radius = 1.0f, float height = 1.0f, int divisions = 8);

void ExportMesh(Mesh mesh, const char* fileName);

// par meshes:
// Optimized. Using par for parametric meshes, using custom algorithms for platonic solids.
// 
// obj meshes:
// Unoptimized. Not worth optimizing given the scope of this project.
// Potential optimizations include 1) generate index buffer and 2) load from binary.
