#include "MainScene.h"
#include "Shader.h"
#include "Mesh.h"
#include "App.h"
#include "Rasterization.h"
#include "ImageUtils.h"

constexpr int IMAGE_SIZE = 512;

Vector3* CopyPositions(Vector3* vertices, size_t vertexCount)
{
	Vector3* positions = new Vector3[vertexCount];
	memcpy(positions, vertices, sizeof(Vector3) * vertexCount);
	return positions;
}

void TransformPositions(Matrix matrix, Vector3* vertices, size_t vertexCount)
{
	for (size_t i = 0; i < vertexCount; i++)
		vertices[i] = matrix * vertices[i];
}

void MainScene::OnLoad()
{
	LoadImage(&mImage, IMAGE_SIZE, IMAGE_SIZE);
	LoadTexture(&mTexture, IMAGE_SIZE, IMAGE_SIZE);
}

void MainScene::OnUnload()
{
	UnloadTexture(&mTexture);
	UnloadImage(&mImage);
}

void MainScene::OnUpdate(float dt)
{
	ClearColor(&mImage, BLACK);
	const size_t vc = gMeshSphere.vertexCount;
	const size_t fc = gMeshSphere.faceCount;
	
	Vector3* positions = CopyPositions(gMeshSphere.positions, vc);
	Matrix rotation = RotateY(TotalTime() * 100.0f * DEG2RAD);
	TransformPositions(rotation, positions, vc);
	
	for (size_t i = 0; i < fc; i++)
		DrawFaceWireframes(&mImage, positions, i, GREEN);
	delete[] positions;
}

void MainScene::OnDraw()
{
	UpdateTexture(mTexture, mImage);

	BindTexture(mTexture);
	BindShader(&gShaderFSQ);
	SendInt("u_tex", 0);
	BindFsq();
	DrawFsq();
	UnbindShader();
	UnbindTexture(mTexture);
}
