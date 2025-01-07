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

Mesh gMeshGround;
Mesh gMeshParticle;
Mesh gMeshTd;

// Copies a par_shapes_mesh to a Mesh
void CopyMesh(Mesh* dst, par_shapes_mesh* src);

void CreateMeshes()
{
	const float unit = 1.0f;

	GenMeshCircle(&gMeshCircle, unit);
	GenMeshSphere(&gMeshSphere, unit);
	GenMeshCube(&gMeshCube, unit, unit, unit);

	GenMeshPlane(&gMeshGround, unit, unit);
	GenMeshHexagon(&gMeshParticle, unit);
	TransformMesh(&gMeshGround, RotateX(-PI * 0.5f));
	TransformMesh(&gMeshParticle, RotateX(-PI * 0.5f));

	GenMeshObj(&gMeshHead, "assets/meshes/head.obj");
	GenMeshObj(&gMeshTd, "assets/meshes/bld_td.obj");

	CreateMesh(&gMeshCircle);
	CreateMesh(&gMeshSphere);
	CreateMesh(&gMeshCube);
	CreateMesh(&gMeshHead);
	CreateMesh(&gMeshGround);
	CreateMesh(&gMeshParticle);
	CreateMesh(&gMeshTd);
}

void DestroyMeshes()
{
	DestroyMesh(&gMeshTd);
	DestroyMesh(&gMeshHead);
	DestroyMesh(&gMeshGround);
	DestroyMesh(&gMeshCube);
	DestroyMesh(&gMeshSphere);
	DestroyMesh(&gMeshParticle);
	DestroyMesh(&gMeshCircle);
}

void CreateMesh(Mesh* mesh)
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

void GenMeshObj(Mesh* mesh, const char* path)
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
}

void GenMeshTriangle(Mesh* mesh, Vector3 v0, Vector3 v1, Vector3 v2)
{
	// Could probably use Mesh directly, although this serves as a reference of how to use par_shapes
	par_shapes_mesh* par = par_shapes_create_empty();
	Vector3 normal = Normalize(Cross(Normalize(v1 - v0), Normalize(v2 - v0)));

	par->npoints = 3;
	par->points = PAR_MALLOC(float, 9);
	par->normals = PAR_MALLOC(float, 9);
	par->tcoords = PAR_MALLOC(float, 6);

	par->triangles = PAR_MALLOC(PAR_SHAPES_T, 3);
	par->ntriangles = 1;

	// Use this to recreate DrawEquilateral(float sideLength)
	//par->points[0] = 0.0f;
	//par->points[1] = 1.0f;
	//par->points[2] = 0.0f;
	//par->points[3] = -1.0f * sinf(PI / 3.0f);
	//par->points[4] = -1.0f * cosf(PI / 3.0f);
	//par->points[5] = 0.0f;
	//par->points[6] = 1.0f * sinf(PI / 3.0f);
	//par->points[7] = -1.0f * cosf(PI / 3.0f);
	//par->points[8] = 0.0f;

	par->points[0] = v0.x;
	par->points[1] = v0.y;
	par->points[2] = v0.z;

	par->points[3] = v1.x;
	par->points[4] = v1.y;
	par->points[5] = v1.z;

	par->points[6] = v2.x;
	par->points[7] = v2.y;
	par->points[8] = v2.z;

	par->normals[0] = normal.x;
	par->normals[1] = normal.y;
	par->normals[2] = normal.z;

	par->normals[3] = normal.x;
	par->normals[4] = normal.y;
	par->normals[5] = normal.z;

	par->normals[6] = normal.x;
	par->normals[7] = normal.y;
	par->normals[8] = normal.z;

	par->tcoords[0] = 0.0f;
	par->tcoords[1] = 0.0f;
	par->tcoords[2] = 1.0f;
	par->tcoords[3] = 0.0f;
	par->tcoords[4] = 0.5f;
	par->tcoords[5] = 1.0f;

	par->triangles[0] = 0;
	par->triangles[1] = 1;
	par->triangles[2] = 2;

	CopyMesh(mesh, par);
	par_shapes_free_mesh(par);
}

void GenMeshHexagon(Mesh* mesh, float radius)
{
	Vector3 position = V3_ZERO;
	Vector3 normal = V3_FORWARD;
	par_shapes_mesh* par = par_shapes_create_disk(radius, 6, &position.x, &normal.x);
	CopyMesh(mesh, par);
	par_shapes_free_mesh(par);
}

void GenMeshCircle(Mesh* mesh, float radius)
{
	Vector3 position = V3_ZERO;
	Vector3 normal = V3_FORWARD;
	par_shapes_mesh* par = par_shapes_create_disk(radius, 32, &position.x, &normal.x);
	CopyMesh(mesh, par);
	par_shapes_free_mesh(par);
}

void GenMeshSemicircle(Mesh* mesh, float radius)
{
	Vector3 position = V3_ZERO;
	Vector3 normal = V3_FORWARD;
	par_shapes_mesh* par = par_shapes_create_half_disk(radius, 16, &position.x, &normal.x);
	CopyMesh(mesh, par);
	par_shapes_free_mesh(par);
}

void GenMeshSphere(Mesh* mesh, float radius)
{
	par_shapes_mesh* par = par_shapes_create_parametric_sphere(8, 8);
	CopyMesh(mesh, par);
	par_shapes_free_mesh(par);
}

void GenMeshHemisphere(Mesh* mesh, float radius)
{
	par_shapes_mesh* par = par_shapes_create_hemisphere(4, 4);
	CopyMesh(mesh, par);
	par_shapes_free_mesh(par);

	// Orient along +Z
	TransformMesh(mesh, RotateX(-PI * 0.5f));
}

void GenMeshCylinder(Mesh* mesh, float radius, float height)
{
	par_shapes_mesh* par = par_shapes_create_cylinder(8, 1);
	CopyMesh(mesh, par);
	par_shapes_free_mesh(par);

	// Center about the origin
	TransformMesh(mesh, Translate(0.0f, 0.0f, -0.5f) * Scale(radius, height, 1.0f));
}

void GenMeshPlane(Mesh* mesh, float width, float height)
{
	par_shapes_mesh* par = par_shapes_create_plane(1, 1);
	CopyMesh(mesh, par);
	par_shapes_free_mesh(par);

	// Center about the origin
	TransformMesh(mesh, Translate(-0.5f, -0.5f, 0.0f) * Scale(width, height, 1.0f));
}

void GenMeshCube(Mesh* mesh, float width, float height, float depth)
{
	float positions[] = {
		-width / 2, -height / 2, depth / 2,
		width / 2, -height / 2, depth / 2,
		width / 2, height / 2, depth / 2,
		-width / 2, height / 2, depth / 2,
		-width / 2, -height / 2, -depth / 2,
		-width / 2, height / 2, -depth / 2,
		width / 2, height / 2, -depth / 2,
		width / 2, -height / 2, -depth / 2,
		-width / 2, height / 2, -depth / 2,
		-width / 2, height / 2, depth / 2,
		width / 2, height / 2, depth / 2,
		width / 2, height / 2, -depth / 2,
		-width / 2, -height / 2, -depth / 2,
		width / 2, -height / 2, -depth / 2,
		width / 2, -height / 2, depth / 2,
		-width / 2, -height / 2, depth / 2,
		width / 2, -height / 2, -depth / 2,
		width / 2, height / 2, -depth / 2,
		width / 2, height / 2, depth / 2,
		width / 2, -height / 2, depth / 2,
		-width / 2, -height / 2, -depth / 2,
		-width / 2, -height / 2, depth / 2,
		-width / 2, height / 2, depth / 2,
		-width / 2, height / 2, -depth / 2
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

void TransformMesh(Mesh* mesh, Matrix transform)
{
	for (size_t i = 0; i < mesh->positions.size(); i++)
		mesh->positions[i] = transform * mesh->positions[i];

	Matrix normal = NormalMatrix(transform);
	for (size_t i = 0; i < mesh->normals.size(); i++)
		mesh->normals[i] = normal * mesh->normals[i];
}

void CopyMesh(Mesh* dst, par_shapes_mesh* src)
{
	dst->count = 3 * src->ntriangles;
	dst->indices.resize(dst->count);
	for (int i = 0; i < dst->count; i++)
		dst->indices[i] = src->triangles[i];

	dst->positions.resize(src->npoints);
	dst->normals.resize(src->npoints);
	dst->tcoords.resize(src->npoints);
	for (int i = 0; i < src->npoints; i++)
	{
		dst->positions[i] = ((Vector3*)src->points)[i];
		dst->normals[i] = ((Vector3*)src->normals)[i];
	}

	if (src->tcoords != nullptr)
	{
		for (int i = 0; i < src->npoints; i++)
			dst->tcoords[i] = ((Vector2*)src->tcoords)[i];
	}
}

// Leaving this in case I want a par_shapes platonics solid in the future (remember to unweld normals with indices = true)
//void GenPlatonic(Mesh* mesh)
//{
//	par_shapes_mesh* par = nullptr;
//	//par = par_shapes_create_cube();
//	//par_shapes_translate(par, -0.5f, -0.5f, -0.5f);
//	par = par_shapes_create_subdivided_sphere(1);
//	par_shapes_unweld(par, true);
//	par_shapes_compute_normals(par);
//
//	mesh->count = par->npoints;
//	mesh->positions.resize(par->npoints);
//	mesh->normals.resize(par->npoints);
//	for (int i = 0; i < par->npoints; i++)
//	{
//		mesh->positions[i] = ((Vector3*)par->points)[i];
//		mesh->normals[i] = ((Vector3*)par->normals)[i];
//	}
//
//	par_shapes_free_mesh(par);
//}
