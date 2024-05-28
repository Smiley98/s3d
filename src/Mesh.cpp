#include "Mesh.h"

#define PAR_SHAPES_IMPLEMENTATION
#include <par_shapes.h>

#include "Math.h"
#include <cstdio>
#include <vector>

GLuint fVaoFsq = GL_NONE;
Mesh gMeshTriangle;
Mesh gMeshCube;
Mesh gMeshDodecahedron;

void CreateMesh(Mesh& mesh, const char* path)
{
	std::vector<Vector3> out_vertices;
	std::vector<Vector2> out_uvs;
	std::vector<Vector3> out_normals;

	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<Vector3> temp_vertices;
	std::vector<Vector2> temp_uvs;
	std::vector<Vector3> temp_normals;

	FILE* file = fopen(path, "r");
	//fopen_s(&file, path, "r");

	if (!file)
	{
		printf("MeshCache: could not open file: %s", path);
	}

	while (true)
	{
		char lineHeader[128];

		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)	//break if the result of the file operation indicates we've reached the end
			break;
		if (strcmp(lineHeader, "v") == 0) //if the first part of the line before spaces is equivalent to "v" we are dealing with a vertex:
		{
			Vector3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0)	//if the first part of the line before spaces is equivalent to "vt" we are dealing with a texture coordinate:
		{
			Vector2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			uv.y = 1.0f - uv.y;
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0)	//if the first part of the line before spaces is equivalent to "vn" we are dealing a vertex normal:
		{
			Vector3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0) //if the first part of the line before spaces is equivalent to "vn" we are dealing with a face:
		{
			//std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
				&vertexIndex[0], &uvIndex[0], &normalIndex[0],
				&vertexIndex[1], &uvIndex[1], &normalIndex[1],
				&vertexIndex[2], &uvIndex[2], &normalIndex[2]);

			for (int i = 0; i < 3; i++) {
				vertexIndices.push_back(vertexIndex[i]);
				uvIndices.push_back(uvIndex[i]);
				normalIndices.push_back(normalIndex[i]);
			}
		}
	}
	fclose(file);

	mesh.vertexCount = vertexIndices.size();
	out_vertices.resize(mesh.vertexCount);
	out_normals.resize(mesh.vertexCount);
	out_uvs.resize(mesh.vertexCount);

	Mesh2 m2;
	size_t vc = out_vertices.size();
	m2.vertexCount = vc;
	assert(vc > 0);

	m2.positions = new Vector3[vc];
	for (size_t i = 0; i < vc; i++)
	{
		Vector3 position = temp_vertices[vertexIndices[i] - 1];
		m2.positions[i] = position;
	}

	if (!temp_normals.empty())
	{
		m2.normals = new Vector3[vc];
		for (size_t i = 0; i < vc; i++)
		{
			Vector3 normal = temp_normals[normalIndices[i] - 1];
			m2.normals[i] = normal;
		}
	}

	if (!temp_uvs.empty())
	{
		m2.tcoords = new Vector2[vc];
		for (size_t i = 0; i < vc; i++)
		{
			Vector2 uv = temp_uvs[uvIndices[i] - 1];
			m2.tcoords[i] = uv;
		}
	}

	if (!vertexIndices.empty())
	{
		// Make an element buffer?
	}

	// For each vertex of each triangle
	for (size_t i = 0; i < mesh.vertexCount; i++) {
		Vector3 position = temp_vertices[vertexIndices[i] - 1];
		Vector3 normal = temp_normals[normalIndices[i] - 1];
		Vector2 uv = temp_uvs[uvIndices[i] - 1];

		out_vertices[i] = position;
		out_normals[i] = normal;
		out_uvs[i] = uv;
	}

	glGenVertexArrays(1, &m2.vao);
	glBindVertexArray(m2.vao);

	glGenBuffers(1, &m2.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m2.vbo);
	glBufferData(GL_ARRAY_BUFFER, vc * sizeof(Vector3), m2.positions, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), nullptr);
	glEnableVertexAttribArray(0);

	if (m2.normals != nullptr)
	{
		glGenBuffers(1, &m2.nbo);
		glBindBuffer(GL_ARRAY_BUFFER, m2.nbo);
		glBufferData(GL_ARRAY_BUFFER, vc * sizeof(Vector3), m2.normals, GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), nullptr);
		glEnableVertexAttribArray(1);
	}

	if (m2.tcoords != nullptr)
	{
		glGenBuffers(1, &m2.tbo);
		glBindBuffer(GL_ARRAY_BUFFER, m2.tbo);
		glBufferData(GL_ARRAY_BUFFER, vc * sizeof(Vector2), m2.tcoords, GL_STATIC_DRAW);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vector2), nullptr);
		glEnableVertexAttribArray(2);
	}

	if (m2.indices != nullptr)
	{
		glGenBuffers(1, &m2.ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m2.ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, vc * sizeof(uint16_t), m2.indices, GL_STATIC_DRAW);
	}
	glBindVertexArray(GL_NONE);

	mesh.vao = m2.vao;
	mesh.vertexCount = m2.vertexCount;

	//glGenVertexArrays(1, &mesh.vao);
	//glGenBuffers(1, &mesh.positions);
	//glGenBuffers(1, &mesh.normals);
	//glGenBuffers(1, &mesh.uvs);
	//
	//glBindBuffer(GL_ARRAY_BUFFER, mesh.positions);
	//glBufferData(GL_ARRAY_BUFFER, out_vertices.size() * sizeof(Vector3), out_vertices.data(), GL_STATIC_DRAW);
	//
	//glBindBuffer(GL_ARRAY_BUFFER, mesh.normals);
	//glBufferData(GL_ARRAY_BUFFER, out_normals.size() * sizeof(Vector3), out_normals.data(), GL_STATIC_DRAW);
	//
	//glBindBuffer(GL_ARRAY_BUFFER, mesh.uvs);
	//glBufferData(GL_ARRAY_BUFFER, out_uvs.size() * sizeof(Vector2), out_uvs.data(), GL_STATIC_DRAW);
	//
	//glBindVertexArray(mesh.vao);
	//glEnableVertexAttribArray(0);
	//glEnableVertexAttribArray(1);
	//glEnableVertexAttribArray(2);
	//
	//glBindBuffer(GL_ARRAY_BUFFER, mesh.positions);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), (void*)(0));
	//
	//glBindBuffer(GL_ARRAY_BUFFER, mesh.normals);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), (void*)(0));
	//
	//glBindBuffer(GL_ARRAY_BUFFER, mesh.uvs);
	//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vector2), (void*)(0));
	//
	//glBindVertexArray(GL_NONE);
}

void DestroyMesh(Mesh& mesh)
{
	glDeleteBuffers(1, &mesh.positions);
	glDeleteBuffers(1, &mesh.normals);
	glDeleteBuffers(1, &mesh.uvs);
	glDeleteVertexArrays(1, &mesh.vao);
	mesh.vao = mesh.positions = mesh.normals = mesh.uvs = GL_NONE;
	// Reset handles to 0 signifying they're invalid.
}

void CreateMeshes()
{
	glGenVertexArrays(1, &fVaoFsq);

	//par_shapes_mesh* dodecahedron = par_shapes_create_dodecahedron();
	//par_shapes_unweld(dodecahedron, true);
	//par_shapes_compute_normals(dodecahedron);

	//float vertices[]
	//{
	//	-0.5f, -0.5f, 0.0f,
	//	 0.5f, -0.5f, 0.0f,
	//	 0.0f,  0.5f, 0.0f
	//};
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

	// vbo is bound to GL_ARRAY_BUFFER so I guess the attribute is assiciated with the
	// array depending on the handle bound to GL_ARRAY_BUFFER.
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), nullptr);

	// Prevent us from accidentally overwriting an attribute of the currently bound vao.
	glBindVertexArray(GL_NONE);
	gMeshTriangle.vertexCount = 3;

	// 8 points before unwelding, 36 points after unwelding;
	// Basically optimal representation vs per-vertex representation
	par_shapes_mesh* cube = par_shapes_create_cube();
	par_shapes_unweld(cube, true);
	par_shapes_compute_normals(cube);

	std::vector<Vector3> positions;
	positions.resize(cube->npoints);
	memcpy(positions.data(), cube->points, sizeof(Vector3) * cube->npoints);

	std::vector<Vector3> normals;
	normals.resize(cube->npoints);
	memcpy(normals.data(), cube->normals, sizeof(Vector3) * cube->npoints);

	CreateMesh(gMeshCube, "assets/meshes/cube.obj");

	//glGenVertexArrays(1, &gMeshCube.vao);
	//glGenBuffers(1, &gMeshCube.positions);
	//glGenBuffers(1, &gMeshCube.normals);
	////glGenBuffers(1, &gMeshCube.uvs);
	////glGenBuffers(1, &gMeshCube.indices);
	//
	//glBindBuffer(GL_ARRAY_BUFFER, gMeshCube.positions);
	//glBufferData(GL_ARRAY_BUFFER, cube->npoints * sizeof(Vector3), cube->points, GL_STATIC_DRAW);
	//
	////glBindBuffer(GL_ARRAY_BUFFER, gMeshCube.normals);
	////glBufferData(GL_ARRAY_BUFFER, cube->npoints * sizeof(Vector3), cube->normals, GL_STATIC_DRAW);
	////
	////glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gMeshCube.indices);
	////glBufferData(GL_ELEMENT_ARRAY_BUFFER, cube->ntriangles * sizeof(uint16_t), cube->triangles, GL_STATIC_DRAW);
	//
	//glBindVertexArray(gMeshCube.vao);
	//glEnableVertexAttribArray(0);
	////glEnableVertexAttribArray(1);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), nullptr);
	////glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), nullptr);
	//glBindVertexArray(GL_NONE);
	//gMeshCube.vertexCount = cube->npoints;
	////gMeshCube.indexCount = cube->ntriangles;
	//par_shapes_free_mesh(cube);
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
	glEnable(GL_DEPTH_TEST);
}

void DrawMesh(const Mesh& mesh)
{
	glBindVertexArray(mesh.vao);
	glDrawArrays(GL_TRIANGLES, 0, mesh.vertexCount);
	glBindVertexArray(GL_NONE);
}
