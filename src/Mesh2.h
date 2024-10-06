#pragma once
#include <glad/glad.h>
#include <par_shapes.h>
#include <fast_obj.h>
#include <vector>
#include "Math.h"

enum PrimitiveShape2
{
	TRIANGLE2,
	SQUARE2,
	CIRCLE2,
	SEMICIRCLE2,

	CUBE2,
	SPHERE2,
	HEMISPHERE2,
	CYLINDER2,

	PLANE_XZ2,
	PLANE_YZ2,
	PLANE_XY2,
	DODECAHEDRON2
};

struct Mesh2
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

Mesh2 CreateMesh(const char* path);
Mesh2 CreateMesh(PrimitiveShape2 shape);

void DestroyMesh(Mesh2& mesh);