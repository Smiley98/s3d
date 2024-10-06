//#define PAR_SHAPES_IMPLEMENTATION
//#define FAST_OBJ_IMPLEMENTATION
#include "Mesh2.h"

Mesh2 LoadFromObj(const fastObjMesh* const obj)
{
	// Do this later because its more complicated.
	// Implement indexed rendering from par first!
	Mesh2 mesh;
	return mesh;
}

Mesh2 LoadFromPar(const par_shapes_mesh* const par)
{
	int indexCount = 3 * par->ntriangles;
	GLuint vao, pbo, nbo, tbo, ebo;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &pbo);
	glGenBuffers(1, &nbo);
	glGenBuffers(1, &tbo);
	glGenBuffers(1, &ebo);

	glBindBuffer(GL_ARRAY_BUFFER, pbo);
	glBufferData(GL_ARRAY_BUFFER, par->npoints * sizeof(Vector3), par->points, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), nullptr);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, nbo);
	glBufferData(GL_ARRAY_BUFFER, par->npoints * sizeof(Vector3), par->normals, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), nullptr);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, tbo);
	glBufferData(GL_ARRAY_BUFFER, par->npoints * sizeof(Vector2), par->tcoords, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vector2), nullptr);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(PAR_SHAPES_T), par->triangles, GL_STATIC_DRAW);

	glBindVertexArray(GL_NONE);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

	Mesh2 mesh;
	mesh.vao = vao;
	mesh.pbo = pbo;
	mesh.nbo = nbo;
	mesh.tbo = tbo;
	mesh.ebo = ebo;
	mesh.indexCount = indexCount;
	return mesh;
}
