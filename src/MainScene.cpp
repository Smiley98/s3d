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
	ClearDepth(&mImage, 1.0f);

	// See if you can cull faces with view-space normals
	// (re-calculating normals via cross-product seems counter-intuitive...)
	// Try rendering red/green for front vs back faces!

	float tt = TotalTime();
	float sx = sinf(tt + 2.0f * PI * 0.333f);
	float sy = sinf(tt + 2.0f * PI * 0.666f);
	float sz = sinf(tt + 2.0f * PI * 1.000f);
	Matrix model =
		Scale(sx * 2.0f, sy * 3.0f, sz * 4.0f) *
		RotateY(TotalTime() * 100.0f * DEG2RAD) *
		Translate(0.0f, 0.0f, 3.0f);

	Matrix view = LookAt({ 0.0f, 0.0f, 10.0f }, V3_ZERO, V3_UP);
	Matrix proj = Perspective(90.0f * DEG2RAD, 1.0f, 0.001f, 10.0f);
	Matrix mvp = model * view * proj;
	DrawMesh(&mImage, gMeshSphere, mvp, model);
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
