//#define PAR_SHAPES_IMPLEMENTATION
//#define FAST_OBJ_IMPLEMENTATION
#include "Mesh2.h"
#include <cstdio>
#include <cassert>

void Upload(Mesh2& mesh);

// TODO long-term -- Implement a mesh optimizer (build an index buffer for objs).
Mesh2 CreateMesh(const char* path)
{
	Mesh2 mesh;
	fastObjMesh* obj = fast_obj_read(path);
	int count = obj->index_count;
	mesh.positions.resize(count);
	mesh.normals.resize(count);

	assert(obj->position_count > 1);
	for (int i = 0; i < count; i++)
	{
		fastObjIndex idx = obj->indices[i];
		Vector3 position = *((Vector3*)obj->positions + idx.p);
		mesh.positions[i] = position;
	}

	assert(obj->normal_count > 1);
	for (int i = 0; i < count; i++)
	{
		fastObjIndex idx = obj->indices[i];
		Vector3 normal = *((Vector3*)obj->normals + idx.n);
		mesh.normals[i] = normal;
	}

	if (obj->texcoord_count > 1)
	{
		mesh.tcoords.resize(count);
		for (int i = 0; i < count; i++)
		{
			fastObjIndex idx = obj->indices[i];
			Vector2 tcoord = *((Vector2*)obj->texcoords + idx.t);
			mesh.tcoords[i] = tcoord;
		}
	}
	else
	{
		printf("**Warning: mesh %s loaded without texture coordinates**\n", path);
	}
	fast_obj_destroy(obj);
	
	mesh.count = count;
	Upload(mesh);
	return mesh;
}

Mesh2 CreateMesh(PrimitiveShape2 shape)
{
	par_shapes_mesh* par = nullptr;
	Vector3 position = V3_ZERO;
	Vector3 normal = V3_FORWARD;
	switch (shape)
	{
	case TRIANGLE2:
		par = par_shapes_create_empty();
	
		par->npoints = 3;
		par->points = PAR_MALLOC(float, 9);
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

	case SQUARE2:
	case PLANE_XY2:
		par = par_shapes_create_plane(1, 1);
		par_shapes_translate(par, -0.5f, -0.5f, 0.0f);
		break;

	case CIRCLE2:
		par = par_shapes_create_disk(1.0f, 32, &position.x, &normal.x);
		break;

	case SEMICIRCLE2:
		par = par_shapes_create_half_disk(1.0f, 16, &position.x, &normal.x);
		break;

	case CUBE2:
		par = par_shapes_create_cube();
		par_shapes_translate(par, -0.5f, -0.5f, -0.5f);
		break;

	case SPHERE2:
		par = par_shapes_create_subdivided_sphere(1);
		break;

	case HEMISPHERE2:
		par = par_shapes_create_hemisphere(4, 4);
		{
			Vector3 axis = V3_RIGHT;
			par_shapes_rotate(par, PI * 0.5f, &axis.x);
		}
		break;

	case CYLINDER2:
		par = par_shapes_create_cylinder(8, 1);
		par_shapes_translate(par, 0.0f, 0.0f, -0.5f);
		break;

	case PLANE_XZ2:
		par = par_shapes_create_plane(1, 1);
		par_shapes_translate(par, -0.5f, -0.5f, 0.0f);
		{
			Vector3 axis = V3_RIGHT;
			par_shapes_rotate(par, -PI * 0.5f, &axis.x);
		}
		break;

	case PLANE_YZ2:
		par = par_shapes_create_plane(1, 1);
		par_shapes_translate(par, -0.5f, -0.5f, 0.0f);
		{
			Vector3 axis = V3_UP;
			par_shapes_rotate(par, PI * 0.5f, &axis.x);
		}
		break;

	case DODECAHEDRON2:
		par = par_shapes_create_dodecahedron();
		break;

	default:
		assert(false, "Invalid par_shapes Mesh Type");
	}
	par_shapes_compute_normals(par);
	
	Mesh2 mesh;
	mesh.count = 3 * par->ntriangles;
	mesh.indices.resize(mesh.count);
	mesh.positions.resize(par->npoints);
	mesh.normals.resize(par->npoints);
	memcpy(mesh.indices.data(), par->triangles, mesh.count * sizeof(uint16_t));
	memcpy(mesh.positions.data(), par->points, par->npoints * sizeof(Vector3));
	memcpy(mesh.normals.data(), par->normals, par->npoints * sizeof(Vector3));
	if (par->tcoords != nullptr)
	{
		mesh.tcoords.resize(par->npoints);
		memcpy(mesh.tcoords.data(), par->tcoords, par->npoints * sizeof(Vector2));
	}

	par_shapes_free_mesh(par);
	Upload(mesh);
	return mesh;
}

void DestroyMesh(Mesh2& mesh)
{
	glDeleteBuffers(1, &mesh.ebo);
	glDeleteBuffers(1, &mesh.tbo);
	glDeleteBuffers(1, &mesh.nbo);
	glDeleteBuffers(1, &mesh.pbo);
	glDeleteVertexArrays(1, &mesh.vao);
	mesh.ebo = mesh.tbo = mesh.nbo = mesh.pbo = mesh.vao = GL_NONE;

	mesh.positions.resize(0);
	mesh.normals.resize(0);
	mesh.tcoords.resize(0);
	mesh.indices.resize(0);
	mesh.count = 0;
}

void Upload(Mesh2& mesh)
{
	GLuint vao, pbo, nbo, tbo, ebo;
	vao = pbo = nbo = tbo = ebo = GL_NONE;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &pbo);
	glBindBuffer(GL_ARRAY_BUFFER, pbo);
	glBufferData(GL_ARRAY_BUFFER, mesh.positions.size() * sizeof(Vector3), mesh.positions.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), nullptr);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &nbo);
	glBindBuffer(GL_ARRAY_BUFFER, nbo);
	glBufferData(GL_ARRAY_BUFFER, mesh.normals.size() * sizeof(Vector3), mesh.normals.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), nullptr);
	glEnableVertexAttribArray(1);

	if (!mesh.tcoords.empty())
	{
		glGenBuffers(1, &tbo);
		glBindBuffer(GL_ARRAY_BUFFER, tbo);
		glBufferData(GL_ARRAY_BUFFER, mesh.tcoords.size() * sizeof(Vector2), mesh.tcoords.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vector2), nullptr);
		glEnableVertexAttribArray(2);
	}

	if (!mesh.indices.empty())
	{
		glGenBuffers(1, &ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(uint16_t), mesh.indices.data(), GL_STATIC_DRAW);
	}

	glBindVertexArray(GL_NONE);
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

	mesh.vao = vao;
	mesh.pbo = pbo;
	mesh.nbo = nbo;
	mesh.tbo = tbo;
	mesh.ebo = ebo;
}
