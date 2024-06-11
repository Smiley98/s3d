#include "MainScene.h"
#include "Shader.h"
#include "App.h"
#include "Rasterization.h"

constexpr int IMAGE_SIZE = 512;

void MainScene::OnLoad()
{
	LoadImage(&mImage, IMAGE_SIZE, IMAGE_SIZE);
	LoadTexture(&mTexture, IMAGE_SIZE, IMAGE_SIZE);
	
	// Works. Might need to flip it!
	//LoadTexture(&mTexHead, gImageDiffuse.width, gImageDiffuse.height);
	//UpdateTexture(mTexHead, gImageDiffuse);
	mMesh = gMeshHead;
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

	Matrix model = Translate(0.0f, 0.0f, 0.0f);
	Matrix view = LookAt({ 0.0f, 0.0f, 5.0f }, V3_ZERO, V3_UP);
	//Matrix proj = Perspective(75.0f * DEG2RAD, 1.0f, 0.001f, 100.0f);
	Matrix proj = Ortho(-1.0f, 1.0f, -1.0f, 1.0f, -10.0f, 10.0f);
	// Pay attention to near/far & view z (depth test might fail)!
	Matrix mvp = model * view * proj;
	DrawMesh(&mImage, mMesh, mvp, model);
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

void MainScene::OnDrawGui()
{
}
