#define PAR_SHAPES_IMPLEMENTATION
#define FAST_OBJ_IMPLEMENTATION
#include <par_shapes.h>
#include <fast_obj.h>

#include "Mesh.h"
#include <cstdio>
#include <cassert>

#define PLATONIC false

Mesh gMeshCircle;
Mesh gMeshSphere;
Mesh gMeshCube;
Mesh gMeshHead;

void Upload(Mesh* mesh);

void GenPlatonic(Mesh* mesh, MeshType type);
void GenParametric(Mesh* mesh, MeshType type);
void GenCube(Mesh* mesh, float width, float height, float length);

void CreateMeshes()
{
	GenParametric(&gMeshCircle, MESH_CIRCLE);
	Upload(&gMeshCircle);

	GenParametric(&gMeshSphere, MESH_SPHERE);
	Upload(&gMeshSphere);

	GenCube(&gMeshCube, 1.0f, 1.0f, 1.0f);
	Upload(&gMeshCube);

	CreateMesh(&gMeshHead, "assets/meshes/head.obj");
}

void DestroyMeshes()
{
	DestroyMesh(&gMeshHead);
	DestroyMesh(&gMeshCube);
	DestroyMesh(&gMeshSphere);
	DestroyMesh(&gMeshCircle);
}

void CreateMesh(Mesh* mesh, const char* path, bool gpu)
{
	fastObjMesh* obj = fast_obj_read(path);
	const int count = obj->index_count;

	assert(obj->position_count > 1);
	mesh->positions.resize(count);
	for (int i = 0; i < count; i++)
		mesh->positions[i] = ((Vector3*)obj->positions)[obj->indices[i].p];
	
	assert(obj->normal_count > 1);
	mesh->normals.resize(count);
	for (int i = 0; i < count; i++)
		mesh->normals[i] = ((Vector3*)obj->normals)[obj->indices[i].n];
	
	if (obj->texcoord_count > 1)
	{
		mesh->tcoords.resize(count);
		for (int i = 0; i < count; i++)
			mesh->tcoords[i] = ((Vector2*)obj->texcoords)[obj->indices[i].t];
	}
	else
	{
		printf("**Warning: mesh %s loaded without texture coordinates**\n", path);
	}
	
	fast_obj_destroy(obj);
	mesh->count = count;

	if (gpu)
		Upload(mesh);
}

void CreateMesh(Mesh* mesh, MeshType type, bool gpu)
{
#if PLATONIC
	if (type == MESH_CUBE || type == MESH_SPHERE)
		GenPlatonic(mesh, type);
#else
	if (type == MESH_CUBE)
		GenCube(mesh, 1.0f, 1.0f, 1.0f);
#endif
	else
		GenParametric(mesh, type);

	if (gpu)
		Upload(mesh);
}

void DestroyMesh(Mesh* mesh)
{
	glDeleteBuffers(1, &mesh->ebo);
	glDeleteBuffers(1, &mesh->tbo);
	glDeleteBuffers(1, &mesh->nbo);
	glDeleteBuffers(1, &mesh->pbo);
	glDeleteVertexArrays(1, &mesh->vao);
	mesh->ebo = mesh->tbo = mesh->nbo = mesh->pbo = mesh->vao = GL_NONE;

	mesh->positions.resize(0);
	mesh->normals.resize(0);
	mesh->tcoords.resize(0);
	mesh->indices.resize(0);
	mesh->count = 0;
}

void Upload(Mesh* mesh)
{
	GLuint vao, pbo, nbo, tbo, ebo;
	vao = pbo = nbo = tbo = ebo = GL_NONE;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &pbo);
	glBindBuffer(GL_ARRAY_BUFFER, pbo);
	glBufferData(GL_ARRAY_BUFFER, mesh->positions.size() * sizeof(Vector3), mesh->positions.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), nullptr);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &nbo);
	glBindBuffer(GL_ARRAY_BUFFER, nbo);
	glBufferData(GL_ARRAY_BUFFER, mesh->normals.size() * sizeof(Vector3), mesh->normals.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), nullptr);
	glEnableVertexAttribArray(1);

	if (!mesh->tcoords.empty())
	{
		glGenBuffers(1, &tbo);
		glBindBuffer(GL_ARRAY_BUFFER, tbo);
		glBufferData(GL_ARRAY_BUFFER, mesh->tcoords.size() * sizeof(Vector2), mesh->tcoords.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vector2), nullptr);
		glEnableVertexAttribArray(2);
	}

	if (!mesh->indices.empty())
	{
		glGenBuffers(1, &ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indices.size() * sizeof(uint16_t), mesh->indices.data(), GL_STATIC_DRAW);
	}

	glBindVertexArray(GL_NONE);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

	mesh->vao = vao;
	mesh->pbo = pbo;
	mesh->nbo = nbo;
	mesh->tbo = tbo;
	mesh->ebo = ebo;
}

void GenPlatonic(Mesh* mesh, MeshType type)
{
	par_shapes_mesh* par = nullptr;
	switch (type)
	{
	case MESH_CUBE:
		par = par_shapes_create_cube();
		par_shapes_translate(par, -0.5f, -0.5f, -0.5f);
		break;

	case MESH_SPHERE:
		par = par_shapes_create_subdivided_sphere(1);
		break;

	default:
		assert(false, "Unsupported platonic solid");
		break;
	}

	par_shapes_unweld(par, true);
	par_shapes_compute_normals(par);

	mesh->count = par->npoints;
	mesh->positions.resize(par->npoints);
	mesh->normals.resize(par->npoints);
	for (int i = 0; i < par->npoints; i++)
	{
		mesh->positions[i] = ((Vector3*)par->points)[i];
		mesh->normals[i] = ((Vector3*)par->normals)[i];
	}

	par_shapes_free_mesh(par);
}

void GenParametric(Mesh* mesh, MeshType type)
{
	par_shapes_mesh* par = nullptr;
	Vector3 position = V3_ZERO;
	Vector3 normal = V3_FORWARD;
	switch (type)
	{
	case MESH_TRIANGLE:
		par = par_shapes_create_empty();

		par->npoints = 3;
		par->points = PAR_MALLOC(float, 9);
		par->normals = PAR_MALLOC(float, 9);
		par->tcoords = PAR_MALLOC(float, 6);

		par->triangles = PAR_MALLOC(PAR_SHAPES_T, 3);
		par->ntriangles = 1;

		par->points[0] = 0.0f;
		par->points[1] = 1.0f;
		par->points[2] = 0.0f;
		par->points[3] = -1.0f * sinf(PI / 3.0f);
		par->points[4] = -1.0f * cosf(PI / 3.0f);
		par->points[5] = 0.0f;
		par->points[6] = 1.0f * sinf(PI / 3.0f);
		par->points[7] = -1.0f * cosf(PI / 3.0f);
		par->points[8] = 0.0f;

		par->normals[0] = 0.0f;
		par->normals[1] = 0.0f;
		par->normals[2] = 1.0f;
		par->normals[3] = 0.0f;
		par->normals[4] = 0.0f;
		par->normals[5] = 1.0f;
		par->normals[6] = 0.0f;
		par->normals[7] = 0.0f;
		par->normals[8] = 1.0f;

		par->tcoords[0] = 0.0f;
		par->tcoords[1] = 0.0f;
		par->tcoords[2] = 1.0f;
		par->tcoords[3] = 0.0f;
		par->tcoords[4] = 0.5f;
		par->tcoords[5] = 1.0f;

		par->triangles[0] = 0;
		par->triangles[1] = 1;
		par->triangles[2] = 2;
		break;

	case MESH_SQUARE:
	case MESH_PLANE_Z:
		par = par_shapes_create_plane(1, 1);
		par_shapes_translate(par, -0.5f, -0.5f, 0.0f);
		break;

	case MESH_CIRCLE:
		par = par_shapes_create_disk(1.0f, 32, &position.x, &normal.x);
		break;

	case MESH_SEMICIRCLE:
		par = par_shapes_create_half_disk(1.0f, 16, &position.x, &normal.x);
		break;

	case MESH_SPHERE:
		par = par_shapes_create_parametric_sphere(8, 8);
		break;

	case MESH_HEMISPHERE:
		par = par_shapes_create_hemisphere(4, 4);
		{
			Vector3 axis = V3_RIGHT;
			par_shapes_rotate(par, PI * 0.5f, &axis.x);
		}
		break;

	case MESH_CYLINDER:
		par = par_shapes_create_cylinder(8, 1);
		par_shapes_translate(par, 0.0f, 0.0f, -0.5f);
		break;

	case MESH_PLANE_Y:
		par = par_shapes_create_plane(1, 1);
		par_shapes_translate(par, -0.5f, -0.5f, 0.0f);
		{
			Vector3 axis = V3_RIGHT;
			par_shapes_rotate(par, -PI * 0.5f, &axis.x);
		}
		break;

	case MESH_PLANE_X:
		par = par_shapes_create_plane(1, 1);
		par_shapes_translate(par, -0.5f, -0.5f, 0.0f);
		{
			Vector3 axis = V3_UP;
			par_shapes_rotate(par, PI * 0.5f, &axis.x);
		}
		break;

	default:
		assert(false, "Unsupported parametric surface");
		break;
	}

	mesh->count = 3 * par->ntriangles;
	mesh->indices.resize(mesh->count);
	for (int i = 0; i < mesh->count; i++)
		mesh->indices[i] = par->triangles[i];

	mesh->positions.resize(par->npoints);
	mesh->normals.resize(par->npoints);
	mesh->tcoords.resize(par->npoints);
	for (int i = 0; i < par->npoints; i++)
	{
		mesh->positions[i] = ((Vector3*)par->points)[i];
		mesh->normals[i] = ((Vector3*)par->normals)[i];
	}

	if (par->tcoords != nullptr)
	{
		for (int i = 0; i < par->npoints; i++)
			mesh->tcoords[i] = ((Vector2*)par->tcoords)[i];
	}

	par_shapes_free_mesh(par);
}

void GenCube(Mesh* mesh, float width, float height, float length)
{
	float positions[] = {
		-width / 2, -height / 2, length / 2,
		width / 2, -height / 2, length / 2,
		width / 2, height / 2, length / 2,
		-width / 2, height / 2, length / 2,
		-width / 2, -height / 2, -length / 2,
		-width / 2, height / 2, -length / 2,
		width / 2, height / 2, -length / 2,
		width / 2, -height / 2, -length / 2,
		-width / 2, height / 2, -length / 2,
		-width / 2, height / 2, length / 2,
		width / 2, height / 2, length / 2,
		width / 2, height / 2, -length / 2,
		-width / 2, -height / 2, -length / 2,
		width / 2, -height / 2, -length / 2,
		width / 2, -height / 2, length / 2,
		-width / 2, -height / 2, length / 2,
		width / 2, -height / 2, -length / 2,
		width / 2, height / 2, -length / 2,
		width / 2, height / 2, length / 2,
		width / 2, -height / 2, length / 2,
		-width / 2, -height / 2, -length / 2,
		-width / 2, -height / 2, length / 2,
		-width / 2, height / 2, length / 2,
		-width / 2, height / 2, -length / 2
	};

	float tcoords[] = {
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f
	};

	float normals[] = {
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f,-1.0f,
		0.0f, 0.0f,-1.0f,
		0.0f, 0.0f,-1.0f,
		0.0f, 0.0f,-1.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f,-1.0f, 0.0f,
		0.0f,-1.0f, 0.0f,
		0.0f,-1.0f, 0.0f,
		0.0f,-1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f
	};

	mesh->positions.resize(24);
	mesh->normals.resize(24);
	mesh->tcoords.resize(24);
	memcpy(mesh->positions.data(), positions, 24 * sizeof(Vector3));
	memcpy(mesh->normals.data(), normals, 24 * sizeof(Vector3));
	memcpy(mesh->tcoords.data(), tcoords, 24 * sizeof(Vector2));

	int k = 0;
	mesh->indices.resize(36);
	for (int i = 0; i < 36; i += 6)
	{
		mesh->indices[i] = 4 * k;
		mesh->indices[i + 1] = 4 * k + 1;
		mesh->indices[i + 2] = 4 * k + 2;
		mesh->indices[i + 3] = 4 * k;
		mesh->indices[i + 4] = 4 * k + 2;
		mesh->indices[i + 5] = 4 * k + 3;

		k++;
	}

	mesh->count = 36;
}
