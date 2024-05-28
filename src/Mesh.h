#pragma once
#include <glad/glad.h>
#include <vector>

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

struct Mesh2
{
	// If we don't use stl, then arrays aren't visible in the debugger :(
	Vector3* positions = nullptr;
	Vector3* normals = nullptr;
	Vector2* tcoords = nullptr;
	uint16_t* indices = nullptr;
	
	// If we use stl vectors, then this object must be passed by reference :(
	//std::vector<Vector3> positions;
	//std::vector<Vector3> normals;
	//std::vector<Vector2> tcoords;
	//std::vector<uint16_t> indices;
	size_t vertexCount = 0;

	// No need in CPU renderer, but easier to store all in one place!
	GLuint vao = GL_NONE;	// Vertex array object
	GLuint vbo = GL_NONE;	// Vertex positions
	GLuint nbo = GL_NONE;	// Vertex normals
	GLuint tbo = GL_NONE;	// Vertex texture coordinates
	GLuint ibo = GL_NONE;	// Vertex indices
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