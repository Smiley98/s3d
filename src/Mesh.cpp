#include "Mesh.h"

#define PAR_SHAPES_IMPLEMENTATION
#include <par_shapes.h>

#include <cstdio>
#include <vector>

GLuint fVaoFsq = GL_NONE;
Mesh gMeshTriangle;
Mesh gMeshCube;
Mesh gMeshDodecahedron;

void CreateMeshCPU(Mesh* mesh, size_t vc, Vector3* positions, Vector3* normals, Vector2* tcoords, uint16_t* indices);
void CreateMeshGPU(Mesh* mesh);
void DestroyMeshCPU(Mesh* mesh);
void DestroyMeshGPU(Mesh* mesh);

void LoadFromPar(Mesh* mesh, par_shapes_mesh* par_mesh);
void LoadFromObj(Mesh* mesh, const char* path);

void CreateMeshes()
{
	glGenVertexArrays(1, &fVaoFsq);

	float vertices[]
	{
		1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f
	};
	int szVertices = 3 * sizeof(Vector3);
	GLuint vbo, tbo, nbo;
	glGenVertexArrays(1, &gMeshTriangle.vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, szVertices, &vertices, GL_STATIC_DRAW);
	glBindVertexArray(gMeshTriangle.vao);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), nullptr);
	glBindVertexArray(GL_NONE);
	gMeshTriangle.vertexCount = 3;

	bool par = false;
	if (par)
	{
		par_shapes_mesh* cube = par_shapes_create_cube();
		par_shapes_unweld(cube, true);
		par_shapes_compute_normals(cube);

		LoadFromPar(&gMeshCube, cube);
		par_shapes_free_mesh(cube);
		CreateMeshGPU(&gMeshCube);
	}
	else
	{
		LoadFromObj(&gMeshCube, "assets/meshes/cube.obj");
	}
}

void DestroyMeshes()
{
	auto Destroy = [](Mesh* mesh)
	{
		DestroyMeshGPU(mesh);
		DestroyMeshCPU(mesh);
	};

	Destroy(&gMeshCube);
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
	glEnable(GL_DEPTH_TEST);
}

void DrawMesh(Mesh mesh)
{
	glBindVertexArray(mesh.vao);
	glDrawArrays(GL_TRIANGLES, 0, mesh.vertexCount);
	glBindVertexArray(GL_NONE);
}

void CreateMeshCPU(Mesh* mesh, size_t vc,
	Vector3* positions, Vector3* normals, Vector2* tcoords, uint16_t* indices)
{
	mesh->vertexCount = vc;
	mesh->positions = new Vector3[vc];
	memcpy(mesh->positions, positions, vc * sizeof(Vector3));

	if (normals != nullptr)
	{
		mesh->normals = new Vector3[vc];
		memcpy(mesh->normals, normals, vc * sizeof(Vector3));
	}

	if (tcoords != nullptr)
	{
		mesh->tcoords = new Vector2[vc];
		memcpy(mesh->tcoords, tcoords, vc * sizeof(Vector2));
	}

	// Double-check whether index count ntriangles or npoints
	if (indices != nullptr)
	{
		mesh->indices = new uint16_t[vc];
		memcpy(mesh->indices, indices, vc * sizeof(uint16_t));
	}
}

void CreateMeshGPU(Mesh* mesh)
{
	glGenVertexArrays(1, &mesh->vao);
	glBindVertexArray(mesh->vao);
	size_t vc = mesh->vertexCount;

	glGenBuffers(1, &mesh->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
	glBufferData(GL_ARRAY_BUFFER, vc * sizeof(Vector3), mesh->positions, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), nullptr);
	glEnableVertexAttribArray(0);

	if (mesh->normals != nullptr)
	{
		glGenBuffers(1, &mesh->nbo);
		glBindBuffer(GL_ARRAY_BUFFER, mesh->nbo);
		glBufferData(GL_ARRAY_BUFFER, vc * sizeof(Vector3), mesh->normals, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), nullptr);
		glEnableVertexAttribArray(1);
	}

	if (mesh->tcoords != nullptr)
	{
		glGenBuffers(1, &mesh->tbo);
		glBindBuffer(GL_ARRAY_BUFFER, mesh->tbo);
		glBufferData(GL_ARRAY_BUFFER, vc * sizeof(Vector2), mesh->tcoords, GL_STATIC_DRAW);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vector2), nullptr);
		glEnableVertexAttribArray(2);
	}

	if (mesh->indices != nullptr)
	{
		glGenBuffers(1, &mesh->ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, vc * sizeof(uint16_t), mesh->indices, GL_STATIC_DRAW);
	}

	glBindVertexArray(GL_NONE);
}

void DestroyMeshCPU(Mesh* mesh)
{
	if (mesh->normals != nullptr)
		delete[] mesh->normals;

	if (mesh->tcoords != nullptr)
		delete[] mesh->tcoords;

	if (mesh->indices != nullptr)
		delete[] mesh->indices;

	delete[] mesh->positions;
	mesh->vertexCount = 0;

	mesh->positions = nullptr;
	mesh->normals = nullptr;
	mesh->tcoords = nullptr;
	mesh->indices = nullptr;
}

void DestroyMeshGPU(Mesh* mesh)
{
	if (mesh->nbo != GL_NONE)
		glDeleteBuffers(1, &mesh->nbo);

	if (mesh->tbo != GL_NONE)
		glDeleteBuffers(1, &mesh->tbo);

	if (mesh->ibo != GL_NONE)
		glDeleteBuffers(1, &mesh->ibo);
	
	glDeleteBuffers(1, &mesh->vbo);
	glDeleteVertexArrays(1, &mesh->vao);

	mesh->vao = GL_NONE;
	mesh->vbo = GL_NONE;
	mesh->nbo = GL_NONE;
	mesh->tbo = GL_NONE;
	mesh->ibo = GL_NONE;
}

void LoadFromPar(Mesh* mesh, par_shapes_mesh* par_mesh)
{
	CreateMeshCPU(mesh, par_mesh->npoints,
		reinterpret_cast<Vector3*>(par_mesh->points),
		reinterpret_cast<Vector3*>(par_mesh->normals),
		reinterpret_cast<Vector2*>(par_mesh->tcoords),
		reinterpret_cast<uint16_t*>(par_mesh->triangles)
	);
}

void LoadFromObj(Mesh* mesh, const char* path)
{
	std::vector<uint16_t> positionIndices, normalIndices, tcoordIndices;

	std::vector<Vector3> obj_positions, obj_normals;
	std::vector<Vector2> obj_tcoords;

	std::vector<Vector3> vtx_positions, vtx_normals;
	std::vector<Vector2> vtx_tcoords;

	FILE* file = fopen(path, "r");
	if (!file)
		printf("Could not open mesh: %s", path);

	while (true)
	{
		char lineHeader[128];
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break;
		if (strcmp(lineHeader, "v") == 0)
		{
			Vector3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			obj_positions.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0)
		{
			Vector2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			//uv.y = 1.0f - uv.y; <-- should fix with STB xD
			obj_tcoords.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0)
		{
			Vector3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			obj_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0)
		{
			int positionIndex[3], tcoordIndex[3], normalIndex[3];
			fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
				&positionIndex[0], &tcoordIndex[0], &normalIndex[0],
				&positionIndex[1], &tcoordIndex[1], &normalIndex[1],
				&positionIndex[2], &tcoordIndex[2], &normalIndex[2]);

			for (int i = 0; i < 3; i++) {
				positionIndices.push_back(positionIndex[i]);
				tcoordIndices.push_back(tcoordIndex[i]);
				normalIndices.push_back(normalIndex[i]);
			}
		}
	}
	fclose(file);
	// Right now my .obj loader needs tcoords on file because it expects them in fscanf.
	// Replace with TinyObj in the future since it'll handle missing data.

	size_t vc = positionIndices.size();
	vtx_positions.resize(vc);
	vtx_normals.resize(vc);
	vtx_tcoords.resize(vc);
	for (size_t i = 0; i < vc; i++)
	{
		Vector3 position = obj_positions[positionIndices[i] - 1];
		Vector3 normal = obj_normals[normalIndices[i] - 1];
		Vector2 uv = obj_tcoords[tcoordIndices[i] - 1];

		vtx_positions[i] = position;
		vtx_normals[i] = normal;
		vtx_tcoords[i] = uv;
	}

	CreateMeshCPU(mesh, vc, vtx_positions.data(), vtx_normals.data(), vtx_tcoords.data(), nullptr);
	CreateMeshGPU(mesh);
}
