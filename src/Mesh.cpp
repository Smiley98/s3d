#define PAR_SHAPES_IMPLEMENTATION
#include "Mesh.h"
#include <cstdio>
#include <vector>

GLuint fVaoFsq = GL_NONE;

Mesh gMeshTriangle;
Mesh gMeshSquare;
Mesh gMeshCircle;
Mesh gMeshSemicircle;

Mesh gMeshCube;
Mesh gMeshSphere;
Mesh gMeshHemisphere;
Mesh gMeshCylinder;

Mesh gMeshPlaneXZ; // unimplemented
Mesh gMeshPlaneYZ; // unimplemented
Mesh gMeshPlaneXY; // unimplemented
Mesh gMeshDodecahedron;

Mesh gMeshHead;

enum PrimitiveShape
{
	TRIANGLE,
	SQUARE,
	CIRCLE,
	SEMICIRCLE,

	CUBE,
	SPHERE,
	HEMISPHERE,
	CYLINDER,

	PLANE_XZ,
	PLANE_YZ,
	PLANE_XY,
	DODECAHEDRON
};

void CreateMeshCPU(Mesh* mesh, size_t vc,
	Vector3* positions, Vector3* normals, Vector2* tcoords,
	Vector3* colors, uint16_t* indices);

void CreateMeshGPU(Mesh* mesh);

void DestroyMeshCPU(Mesh* mesh);
void DestroyMeshGPU(Mesh* mesh);

par_shapes_mesh* LoadPrimitive(PrimitiveShape shape);

size_t LoadObj(const char* path,
	std::vector<Vector3>* vtx_positions,
	std::vector<Vector3>* vtx_normals,
	std::vector<Vector2>* vtx_tcoords);

void CreateMeshes()
{
	// FSQ test
	glGenVertexArrays(1, &fVaoFsq);

	par_shapes_mesh* triangle = LoadPrimitive(TRIANGLE);
	CreateMeshPar(&gMeshTriangle, triangle);
	par_shapes_free_mesh(triangle);

	par_shapes_mesh* square = LoadPrimitive(SQUARE);
	CreateMeshPar(&gMeshSquare, square);
	par_shapes_free_mesh(square);

	par_shapes_mesh* circle = LoadPrimitive(CIRCLE);
	CreateMeshPar(&gMeshCircle, circle);
	par_shapes_free_mesh(circle);

	par_shapes_mesh* semicircle = LoadPrimitive(SEMICIRCLE);
	CreateMeshPar(&gMeshSemicircle, semicircle);
	par_shapes_free_mesh(semicircle);

	par_shapes_mesh* cube = LoadPrimitive(CUBE);
	CreateMeshPar(&gMeshCube, cube);
	par_shapes_free_mesh(cube);

	par_shapes_mesh* sphere = LoadPrimitive(SPHERE);
	CreateMeshPar(&gMeshSphere, sphere);
	par_shapes_free_mesh(sphere);

	par_shapes_mesh* hemisphere = LoadPrimitive(HEMISPHERE);
	CreateMeshPar(&gMeshHemisphere, hemisphere);
	par_shapes_free_mesh(hemisphere);

	par_shapes_mesh* cylinder = LoadPrimitive(CYLINDER);
	CreateMeshPar(&gMeshCylinder, cylinder);
	par_shapes_free_mesh(cylinder);

	par_shapes_mesh* dodecahedron = LoadPrimitive(DODECAHEDRON);
	CreateMeshPar(&gMeshDodecahedron, dodecahedron);
	par_shapes_free_mesh(dodecahedron);

	CreateMeshObj(&gMeshHead, "assets/meshes/head.obj");
}

void DestroyMeshes()
{
	DestroyMesh(&gMeshHead);

	DestroyMesh(&gMeshDodecahedron);
	//DestroyMesh(&gMeshPlaneXY);
	//DestroyMesh(&gMeshPlaneYZ);
	//DestroyMesh(&gMeshPlaneXZ);

	DestroyMesh(&gMeshCylinder);
	DestroyMesh(&gMeshHemisphere);
	DestroyMesh(&gMeshSphere);
	DestroyMesh(&gMeshCube);

	DestroyMesh(&gMeshSemicircle);
	DestroyMesh(&gMeshCircle);
	DestroyMesh(&gMeshSquare);
	DestroyMesh(&gMeshTriangle);

	glDeleteVertexArrays(1, &fVaoFsq);
}

void CreateMesh(Mesh* mesh, size_t vc, Vector3* positions, Vector3* normals, Vector2* tcoords, Vector3* colors, uint16_t* indices)
{
	CreateMeshCPU(mesh, vc, positions, normals, tcoords, colors, indices);
	CreateMeshGPU(mesh);
}

void CreateMeshObj(Mesh* mesh, const char* file)
{
	std::vector<Vector3> vtx_positions;
	std::vector<Vector3> vtx_normals;
	std::vector<Vector2> vtx_tcoords;
	size_t vertex_count = LoadObj(file, &vtx_positions, &vtx_normals, &vtx_tcoords);
	CreateMeshCPU(mesh, vertex_count,
		vtx_positions.data(),
		vtx_normals.data(),
		vtx_tcoords.data(),
		nullptr, nullptr);
	CreateMeshGPU(mesh);
}

void CreateMeshPar(Mesh* mesh, par_shapes_mesh* par_mesh)
{
	CreateMeshCPU(mesh, par_mesh->npoints,
		reinterpret_cast<Vector3*>(par_mesh->points),
		reinterpret_cast<Vector3*>(par_mesh->normals),
		reinterpret_cast<Vector2*>(par_mesh->tcoords),
		nullptr,/*colors*/
		reinterpret_cast<uint16_t*>(par_mesh->triangles)
	);
	CreateMeshGPU(mesh);
}

void DestroyMesh(Mesh* mesh)
{
	DestroyMeshGPU(mesh);
	DestroyMeshCPU(mesh);
}

void CopyMesh(Mesh src, Mesh* dst)
{
	CreateMeshCPU(dst, src.vertexCount, src.positions, src.normals, src.tcoords, src.colors, src.indices);
	CreateMeshGPU(dst);
}

void DrawMesh(Mesh mesh)
{
	glBindVertexArray(mesh.vao);
	glDrawArrays(GL_TRIANGLES, 0, mesh.vertexCount);
	glBindVertexArray(GL_NONE);
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

void GenTriangle(Mesh* mesh, Vector3 v0, Vector3 v1, Vector3 v2)
{
	Vector3 n = Cross(Normalize(v1 - v0), Normalize(v2 - v0));
	Vector3 positions[3] = { v0, v1, v2 };
	Vector3 normals[3] = { n, n, n };
	CreateMesh(mesh, 3, positions, normals, nullptr, nullptr, nullptr);
}

void GenEquilateral(Mesh* mesh)
{
	par_shapes_mesh* shape = LoadPrimitive(TRIANGLE);
	CreateMeshPar(mesh, shape);
	par_shapes_free_mesh(shape);
}

void GenSquare(Mesh* mesh)
{
	par_shapes_mesh* shape = LoadPrimitive(SQUARE);
	CreateMeshPar(mesh, shape);
	par_shapes_free_mesh(shape);
}

void GenCircle(Mesh* mesh)
{
	par_shapes_mesh* shape = LoadPrimitive(CIRCLE);
	CreateMeshPar(mesh, shape);
	par_shapes_free_mesh(shape);
}

void GenSemicircle(Mesh* mesh)
{
	par_shapes_mesh* shape = LoadPrimitive(SEMICIRCLE);
	CreateMeshPar(mesh, shape);
	par_shapes_free_mesh(shape);
}

void GenCube(Mesh* mesh)
{
	par_shapes_mesh* shape = LoadPrimitive(CUBE);
	CreateMeshPar(mesh, shape);
	par_shapes_free_mesh(shape);
}

void GenSphere(Mesh* mesh)
{
	par_shapes_mesh* shape = LoadPrimitive(SPHERE);
	CreateMeshPar(mesh, shape);
	par_shapes_free_mesh(shape);
}

void GenHemisphere(Mesh* mesh)
{
	par_shapes_mesh* shape = LoadPrimitive(HEMISPHERE);
	CreateMeshPar(mesh, shape);
	par_shapes_free_mesh(shape);
}

void GenCylinder(Mesh* mesh)
{
	par_shapes_mesh* shape = LoadPrimitive(CYLINDER);
	CreateMeshPar(mesh, shape);
	par_shapes_free_mesh(shape);
}

void GenPlaneXZ(Mesh* mesh)
{
	par_shapes_mesh* shape = LoadPrimitive(PLANE_XZ);
	CreateMeshPar(mesh, shape);
	par_shapes_free_mesh(shape);
}

void GenPlaneYZ(Mesh* mesh)
{
	par_shapes_mesh* shape = LoadPrimitive(PLANE_YZ);
	CreateMeshPar(mesh, shape);
	par_shapes_free_mesh(shape);
}

void GenPlaneXY(Mesh* mesh)
{
	par_shapes_mesh* shape = LoadPrimitive(PLANE_XY);
	CreateMeshPar(mesh, shape);
	par_shapes_free_mesh(shape);
}

void GenDodecahedron(Mesh* mesh)
{
	par_shapes_mesh* shape = LoadPrimitive(DODECAHEDRON);
	CreateMeshPar(mesh, shape);
	par_shapes_free_mesh(shape);
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

par_shapes_mesh* LoadPrimitive(PrimitiveShape shape)
{
	// No longer translating plane & cube to show how they're generated by default.
	par_shapes_mesh* mesh = nullptr;
	Vector3 position = V3_ZERO;
	Vector3 normal = V3_FORWARD;
	switch (shape)
	{
	case TRIANGLE:
		mesh = par_shapes_create_empty();
		
		mesh->points = PAR_MALLOC(float, 9);
		mesh->npoints = 3;

		mesh->triangles = PAR_MALLOC(PAR_SHAPES_T, 3);
		mesh->ntriangles = 1;

		mesh->points[0] = 0.0f;
		mesh->points[1] = 1.0f;
		mesh->points[2] = 0.0f;

		mesh->points[3] = -1.0f * sinf(PI / 3.0f);
		mesh->points[4] = -1.0f * cosf(PI / 3.0f);
		mesh->points[5] =  0.0f;

		mesh->points[6] =  1.0f * sinf(PI / 3.0f);
		mesh->points[7] = -1.0f * cosf(PI / 3.0f);
		mesh->points[8] =  0.0f;

		mesh->triangles[0] = 0;
		mesh->triangles[1] = 1;
		mesh->triangles[2] = 2;
		break;

	case SQUARE:
		mesh = par_shapes_create_plane(1, 1);
		par_shapes_translate(mesh, -0.5f, -0.5f, 0.0f);
		break;

	case CIRCLE:
		mesh = par_shapes_create_disk(1.0f, 32, &position.x, &normal.x);
		break;

	case SEMICIRCLE:
		mesh = mesh = par_shapes_create_half_disk(1.0f, 16, &position.x, &normal.x);
		break;

	case CUBE:
		mesh = par_shapes_create_cube();
		par_shapes_translate(mesh, -0.5f, -0.5f, -0.5f);
		break;

	case SPHERE:
		mesh = par_shapes_create_subdivided_sphere(1);
		break;

	case HEMISPHERE:
		mesh = par_shapes_create_hemisphere(4, 4);
		{
			Vector3 axis = V3_RIGHT;
			par_shapes_rotate(mesh, PI * 0.5f, &axis.x);
		}
		break;

	case CYLINDER:
		mesh = par_shapes_create_cylinder(8, 1);
		par_shapes_translate(mesh, 0.0f, 0.0f, -0.5f);
		break;

	case PLANE_XZ:
		mesh = par_shapes_create_plane(1, 1);
		par_shapes_translate(mesh, -0.5f, -0.5f, 0.0f);
		{
			Vector3 axis = V3_RIGHT;
			par_shapes_rotate(mesh, -PI * 0.5f, &axis.x);
		}
		break;

	case PLANE_YZ:
		mesh = par_shapes_create_plane(1, 1);
		par_shapes_translate(mesh, -0.5f, -0.5f, 0.0f);
		{
			Vector3 axis = V3_UP;
			par_shapes_rotate(mesh, PI * 0.5f, &axis.x);
		}
		break;

	case PLANE_XY:
		mesh = par_shapes_create_plane(1, 1);
		par_shapes_translate(mesh, -0.5f, -0.5f, 0.0f);
		break;

	case DODECAHEDRON:
		mesh = par_shapes_create_dodecahedron();
		break;

	default:
		assert(false, "Invalid par_shapes Mesh Type");
	}

	// Hemisphere and Cylinder seem to have broken tcoords...
	// These are just debug shapes so just delete no matter what xD
	if (mesh->tcoords != nullptr)
	{
		PAR_FREE(mesh->tcoords);
		mesh->tcoords = nullptr;
	}

	par_shapes_unweld(mesh, true);
	par_shapes_compute_normals(mesh);
	return mesh;
}

size_t LoadObj(const char* path,
	std::vector<Vector3>* vtx_positions, std::vector<Vector3>* vtx_normals, std::vector<Vector2>* vtx_tcoords)
{
	std::vector<uint16_t> positionIndices, normalIndices, tcoordIndices;

	std::vector<Vector3> obj_positions, obj_normals;
	std::vector<Vector2> obj_tcoords;

	FILE* file = fopen(path, "r");
	if (!file)
	{
		assert(false, "Could not open mesh");
		return 0;
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

	size_t vc = positionIndices.size();
	vtx_positions->resize(vc);
	vtx_normals->resize(vc);
	vtx_tcoords->resize(vc);
	for (size_t i = 0; i < vc; i++)
	{
		Vector3 position = obj_positions[positionIndices[i] - 1];
		Vector3 normal = obj_normals[normalIndices[i] - 1];
		Vector2 uv = obj_tcoords[tcoordIndices[i] - 1];

		(*vtx_positions)[i] = position;
		(*vtx_normals)[i] = normal;
		(*vtx_tcoords)[i] = uv;
	}

	return vc;
}
