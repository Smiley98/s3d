#pragma once
#include <glad/glad.h>
#include <vector>
#include "Math.h"

//enum MeshType
//{
//	MESH_TRIANGLE,	// Faces +Z
//	MESH_SQUARE,	// Faces +Z
//	MESH_HEXAGON,	// Faces +Z
//	MESH_CIRCLE,
//	MESH_SEMICIRCLE,
//
//	MESH_CUBE,
//	MESH_SPHERE,
//	MESH_HEMISPHERE,
//	MESH_CYLINDER,
//
//	MESH_PLANE_Z, // Faces +Z
//	MESH_PLANE_Y, // Faces +Y
//	MESH_PLANE_X, // Faces +X
//};

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

// 2D primitives
extern Mesh gMeshCircle;// Unit circle r = 1

// 3D primitives
extern Mesh gMeshSphere;// Unit sphere r = 1
extern Mesh gMeshCube;	// Unit cube L x W x H = 1

// Imported meshes for demos
extern Mesh gMeshHead;

// Imported meshes for main game
extern Mesh gMeshGround;	// 1x1 PLANE_XZ (faces +Y)
extern Mesh gMeshParticle;	// Unit hexagon r = 1, faces +Y
extern Mesh gMeshTd;		// 10 x 10 x 50

void CreateMeshes();	// Create all global meshes
void DestroyMeshes();	// Destroy all global meshes

void CreateMesh(Mesh* mesh);	// Allocate mesh on the GPU
void DestroyMesh(Mesh* mesh);	// Deallocate mesh on the GPU

// Transforms the positions & normals of a mesh on the CPU
void TransformMesh(Mesh* mesh, Matrix transform);

void GenMeshObj(Mesh* mesh, const char* path);

void GenMeshTriangle(Mesh* mesh, Vector3 v0, Vector3 v1, Vector3 v2);
void GenMeshHexagon(Mesh* mesh, float radius);
void GenMeshCircle(Mesh* mesh, float radius);
void GenMeshSemicircle(Mesh* mesh, float radius);

void GenMeshSphere(Mesh* mesh, float radius);
void GenMeshHemisphere(Mesh* mesh, float radius);				// Faces +Z
void GenMeshCylinder(Mesh* mesh, float radius, float height);	// Faces +Z
void GenMeshPlane(Mesh* mesh, float width, float height);		// Faces +Z
void GenMeshCube(Mesh* mesh, float width, float height, float depth);

// par meshes:
// Optimized. Using par for parametric meshes, using custom algorithms for platonic solids.
// 
// obj meshes:
// Unoptimized. Not worth optimizing given the scope of this project.
// Potential optimizations include 1) generate index buffer and 2) load from binary.
