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
	for (size_t i = 0; i < gMeshSphere.faceCount; i++)
		DrawFaceWireframes(&mImage, gMeshSphere.positions, i, RED);
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
