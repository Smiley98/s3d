#pragma once
#include <glad/glad.h>

struct Mesh
{
	size_t vertexCount = 0;
	//size_t indexCount = 0;
	GLuint vao;

	GLuint positions;
	GLuint uvs;
	GLuint normals;
	//GLuint indices;
};

extern Mesh gMeshTriangle;
extern Mesh gMeshCube;
extern Mesh gMeshDodecahedron;

void CreateMesh(Mesh& mesh, const char* path);
void DestroyMesh(Mesh& mesh);

// Create more shapes with par_shapes!
void CreateMeshes();
void DestroyMeshes();

void BindFsq();
void DrawFsq();

void DrawMesh(const Mesh& mesh);
//void DrawMeshIndexed(const Mesh& mesh);