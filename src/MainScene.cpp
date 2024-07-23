#include "MainScene.h"
#include "Shader.h"
#include "Mesh.h"
#include "App.h"
#include "Rasterization.h"
#include "ImageUtils.h"

constexpr int IMAGE_SIZE = 512;

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

	Mesh copy;
	copy.positions = new Vector3[gMeshHead.vertexCount];
	memcpy(copy.positions, gMeshHead.positions, sizeof(Vector3) * gMeshHead.vertexCount);

	for (size_t i = 0; i < gMeshHead.vertexCount; i++)
		copy.positions[i] = RotateY(TotalTime() * 100.0f * DEG2RAD) * copy.positions[i];

	for (size_t i = 0; i < gMeshHead.faceCount; i++)
		DrawFaceWireframes(&mImage, copy, i, RED);

	delete[] copy.positions;
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
