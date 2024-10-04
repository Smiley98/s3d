#pragma once
#include <glad/glad.h>
#include <par_shapes.h>
#include "Math.h"
// Mesh notes:
// 
// -Currently using native arrays
//	Pros are no accidental copies
//	Cons are no memory safety & invisible in the debugger
// 
// -Currently copying all data from par_shapes_mesh to Mesh
//	Pros are legibility
//	Cons are needless copying
// 
// -Probably going to leave unchanged as things currently work well.
// -If I were to re-write this I'd probably store a par_shapes_mesh.
// -Using STL means unavoidable copies.
// -If I want both STL & no copies, I'll have to write my own mesh generator.
// 
// -Could incorporate an index buffer since par_shapes generates indices,
//  but my obj loader does not so I must stick with naive vertices.
//
// -TLDR to improve this I should migrate to tiny_obj / 
//  whatever other quality libraries are out there instead of this...

struct Mesh
{
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
// TODO -- use par_shapes_weld to optimize meshes, then render using indices!
// (This might be a lot of work and is just an optimization, so chill on this).
// (Would also give me an excuse to store CPU values in stl vector for visibility).

struct EBO
{
	par_shapes_mesh* mesh;
	GLuint vao = GL_NONE;
	GLuint vbo = GL_NONE;
	GLuint ebo = GL_NONE;
};

// Imported meshes
extern Mesh gMeshHead;

void CreateMeshes();
void DestroyMeshes();

void EboTest(EBO* ebo);
void EboDraw(EBO ebo);

// Deallocate all of mesh's memory on CPU & GPU
void DestroyMesh(Mesh& mesh);

// Create mesh from data
void CreateMesh(Mesh& mesh, size_t vc, Vector3* positions, Vector3* normals, Vector2* tcoords, Vector3* colors, uint16_t* indices);

// Create mesh from obj file
void CreateMeshObj(Mesh& mesh, const char* file);

// Create mesh from par_shapes
void CreateMeshPar(Mesh& mesh, par_shapes_mesh* par_mesh);

// Copy mesh from source to destination
void CopyMesh(Mesh src, Mesh& dst);

void GenTriangle(Mesh& mesh, Vector3 v0, Vector3 v1, Vector3 v2);
void GenEquilateral(Mesh& mesh);
void GenSquare(Mesh& mesh);
void GenCircle(Mesh& mesh);
void GenSemicircle(Mesh& mesh);

void GenCube(Mesh& mesh);
void GenSphere(Mesh& mesh);
void GenHemisphere(Mesh& mesh);
void GenCylinder(Mesh& mesh);

void GenPlaneXZ(Mesh& mesh);
void GenPlaneYZ(Mesh& mesh);
void GenPlaneXY(Mesh& mesh);
void GenDodecahedron(Mesh& mesh);

// Replaced with mesh generation
//extern Mesh gMeshTriangle;
//extern Mesh gMeshSquare;
//extern Mesh gMeshCircle;
//extern Mesh gMeshSemicircle;
//
//extern Mesh gMeshCube;
//extern Mesh gMeshSphere;
//extern Mesh gMeshHemisphere;
//extern Mesh gMeshCylinder;
//
//extern Mesh gMeshPlaneXZ;
//extern Mesh gMeshPlaneYZ;
//extern Mesh gMeshPlaneXY;
//extern Mesh gMeshDodecahedron;