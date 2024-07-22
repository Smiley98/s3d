#include "Mesh.h"

#define PAR_SHAPES_IMPLEMENTATION
#include <par_shapes.h>

#include <cstdio>
#include <vector>

GLuint fVaoFsq = GL_NONE;
Mesh gMeshTriangle;
Mesh gMeshCube;
Mesh gMeshSphere;
Mesh gMeshHead;
Mesh gMeshDodecahedron;

void CreateMeshCPU(Mesh* mesh, size_t vc,
	Vector3* positions, Vector3* normals, Vector2* tcoords,
	Vector3* colors, uint16_t* indices);

void CreateMeshGPU(Mesh* mesh);
void DestroyMeshCPU(Mesh* mesh);
void DestroyMeshGPU(Mesh* mesh);

void LoadFromPar(Mesh* mesh, par_shapes_mesh* par_mesh);
void LoadFromObj(Mesh* mesh, const char* path);

void CreateMeshes()
{
	glGenVertexArrays(1, &fVaoFsq);
}

void DestroyMeshes()
{
	glDeleteVertexArrays(1, &fVaoFsq);
}

void BindFsq()
{
	glBindVertexArray(fVaoFsq);
}

void DrawFsq()
{
	glDisable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(GL_NONE);
	glEnable(GL_DEPTH_TEST);
}

void DrawMesh(Mesh mesh)
{
	glBindVertexArray(mesh.vao);
	glDrawArrays(GL_TRIANGLES, 0, mesh.vertexCount);
	glBindVertexArray(GL_NONE);
}

void CreateMeshCPU(Mesh* mesh, size_t vc,
	Vector3* positions, Vector3* normals, Vector2* tcoords, Vector3* colors, uint16_t* indices)
{

}

void CreateMeshGPU(Mesh* mesh)
{

}

void DestroyMeshCPU(Mesh* mesh)
{

}

void DestroyMeshGPU(Mesh* mesh)
{

}

void LoadFromPar(Mesh* mesh, par_shapes_mesh* par_mesh)
{
	
}

void LoadFromObj(Mesh* mesh, const char* path)
{
	
}
