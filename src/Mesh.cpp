#include "Mesh.h"

#define PAR_SHAPES_IMPLEMENTATION
#include <par_shapes.h>

#include <cstdio>
#include <vector>

GLuint fVaoFsq = GL_NONE;
Mesh gMeshTriangle;
Mesh gMeshCube;
Mesh gMeshCube2;
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
	// Full-screen quad (ignore this, its vertices are created within its vertex shader)
	glGenVertexArrays(1, &fVaoFsq);

	// Triangle vertex positions (done manually)
	float vertices[]
	{
		0.5f, -0.5f, 0.0f,
		0.0f, 0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f
	};
	CreateMeshCPU(&gMeshTriangle, 3, (Vector3*)vertices, nullptr, nullptr, nullptr, nullptr);
	CreateMeshGPU(&gMeshTriangle);
	
	// Cube mesh generation via par_shapes library
	par_shapes_mesh* cube = par_shapes_create_cube();
	par_shapes_unweld(cube, true);
	par_shapes_compute_normals(cube);
	
	// Translate cube to centre, rotate cube 45 degrees about XY, and scale by 25%
	Vector3 axis = Normalize(V3_RIGHT + V3_UP);
	par_shapes_translate(cube, -0.5f, -0.5f, -0.5f);
	par_shapes_rotate(cube, 45.0f * DEG2RAD, &axis.x);
	par_shapes_scale(cube, 1.25f, 1.25f, 1.25f);
	LoadFromPar(&gMeshCube, cube);
	par_shapes_free_mesh(cube);
	CreateMeshGPU(&gMeshCube);

	// Sphere mesh generation via par_shapes library
	par_shapes_mesh* sphere = par_shapes_create_subdivided_sphere(1);
	par_shapes_unweld(sphere, true);
	par_shapes_compute_normals(sphere);
	LoadFromPar(&gMeshSphere, sphere);
	par_shapes_free_mesh(sphere);
	CreateMeshGPU(&gMeshSphere);

	LoadFromObj(&gMeshHead, "assets/meshes/head.obj");
}

void DestroyMeshes()
{
	auto Destroy = [](Mesh* mesh)
	{
		DestroyMeshGPU(mesh);
		DestroyMeshCPU(mesh);
	};

	Destroy(&gMeshTriangle);
	Destroy(&gMeshCube);
	Destroy(&gMeshSphere);
	Destroy(&gMeshHead);
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
	assert(vc % 3 == 0);
	mesh->vertexCount = vc;
	mesh->faceCount = vc / 3;

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

	if (colors != nullptr)
	{
		mesh->colors = new Vector3[vc];
		memcpy(mesh->colors, colors, vc * sizeof(Vector3));
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

	if (mesh->colors != nullptr)
	{
		glGenBuffers(1, &mesh->cbo);
		glBindBuffer(GL_ARRAY_BUFFER, mesh->cbo);
		glBufferData(GL_ARRAY_BUFFER, vc * sizeof(Vector3), mesh->colors, GL_STATIC_DRAW);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), nullptr);
		glEnableVertexAttribArray(3);
	}

	// Double-check if ebo is associated with vao. Unbind if not!
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

	if (mesh->colors != nullptr)
		delete[] mesh->colors;

	if (mesh->indices != nullptr)
		delete[] mesh->indices;
	delete[] mesh->positions;

	mesh->positions = nullptr;
	mesh->normals = nullptr;
	mesh->tcoords = nullptr;
	mesh->colors = nullptr;
	mesh->indices = nullptr;

	mesh->vertexCount = 0;
	mesh->faceCount = 0;
}

void DestroyMeshGPU(Mesh* mesh)
{
	if (mesh->nbo != GL_NONE)
		glDeleteBuffers(1, &mesh->nbo);

	if (mesh->tbo != GL_NONE)
		glDeleteBuffers(1, &mesh->tbo);

	if (mesh->cbo != GL_NONE)
		glDeleteBuffers(1, &mesh->cbo);

	if (mesh->ibo != GL_NONE)
		glDeleteBuffers(1, &mesh->ibo);

	glDeleteBuffers(1, &mesh->vbo);
	glDeleteVertexArrays(1, &mesh->vao);

	mesh->vao = GL_NONE;
	mesh->vbo = GL_NONE;
	mesh->nbo = GL_NONE;
	mesh->tbo = GL_NONE;
	mesh->cbo = GL_NONE;
	mesh->ibo = GL_NONE;
}

void LoadFromPar(Mesh* mesh, par_shapes_mesh* par_mesh)
{
	CreateMeshCPU(mesh, par_mesh->npoints,
		reinterpret_cast<Vector3*>(par_mesh->points),
		reinterpret_cast<Vector3*>(par_mesh->normals),
		reinterpret_cast<Vector2*>(par_mesh->tcoords),
		nullptr,/*colors*/
		reinterpret_cast<uint16_t*>(par_mesh->triangles)
	);
}

void LoadFromObj(Mesh* mesh, const char* path)
{
	// Indices of values that make up a face
	// (ie face 1 uses position 69, normal 420, and tcoord 1337)
	std::vector<uint16_t> positionIndices, normalIndices, tcoordIndices;

	// Values read top-down from file
	std::vector<Vector3> obj_positions, obj_normals;
	std::vector<Vector2> obj_tcoords;

	// Values organized face-by-face
	std::vector<Vector3> vtx_positions, vtx_normals;
	std::vector<Vector2> vtx_tcoords;

	FILE* file = fopen(path, "r");
	if (!file)
	{
		assert(false, "Could not open mesh");
		return;
	}

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
			// A face needs 3 vertices.
			// Each vertex has a position, normal, and tcoord!
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

	size_t vc = positionIndices.size();
	vtx_positions.resize(vc);
	vtx_normals.resize(vc);
	vtx_tcoords.resize(vc);
	for (size_t i = 0; i < vc; i++)
	{
		// let position = file positions at position index i - 1
		// let normal = file normals at normal index i - 1
		// let tcoord = file tcoords at tcoord index i - 1

		// set vertex position at i to position
		// set vertex normal at i to normal
		// set vertex tcoord at i to tcoord
	}

	CreateMeshCPU(mesh, vc, vtx_positions.data(), vtx_normals.data(),
		vtx_tcoords.data(), nullptr, nullptr);
	CreateMeshGPU(mesh);
}
