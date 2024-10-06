//#define PAR_SHAPES_IMPLEMENTATION
//#define FAST_OBJ_IMPLEMENTATION
#include "Mesh2.h"
#include <cassert>

Mesh2 LoadFromObj(const fastObjMesh* const obj)
{
	// Not worth trying to figure out how to render objs with indexing xD
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

// TODO -- Improve rasterization scene by storing a Primitive enum instead of entire functions
Mesh2 LoadPrimitive2(PrimitiveShape2 shape)
{
	par_shapes_mesh* par = nullptr;
	Vector3 position = V3_ZERO;
	Vector3 normal = V3_FORWARD;
	switch (shape)
	{
	//case TRIANGLE:
	//	par = par_shapes_create_empty();
	//
	//	par->points = PAR_MALLOC(float, 9);
	//	par->npoints = 3;
	//
	//	par->triangles = PAR_MALLOC(PAR_SHAPES_T, 3);
	//	par->ntriangles = 1;
	//
	//	par->points[0] = 0.0f;
	//	par->points[1] = 1.0f;
	//	par->points[2] = 0.0f;
	//
	//	par->points[3] = -1.0f * sinf(PI / 3.0f);
	//	par->points[4] = -1.0f * cosf(PI / 3.0f);
	//	par->points[5] = 0.0f;
	//
	//	par->points[6] = 1.0f * sinf(PI / 3.0f);
	//	par->points[7] = -1.0f * cosf(PI / 3.0f);
	//	par->points[8] = 0.0f;
	//
	//	par->triangles[0] = 0;
	//	par->triangles[1] = 1;
	//	par->triangles[2] = 2;
	//	break;

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

	Mesh2 mesh = LoadFromPar(par);
	par_shapes_free_mesh(par);
	return mesh;
}
