#include "MainScene.h"
#include "Shader.h"
#include "App.h"
#include "Rasterization.h"

constexpr int IMAGE_SIZE = 512;

void MainScene::OnLoad()
{
	LoadImage(&mImage, IMAGE_SIZE, IMAGE_SIZE);
	LoadTexture(&mTexture, IMAGE_SIZE, IMAGE_SIZE);
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

	float tt = TotalTime();
	Matrix translation = MatrixIdentity();//Translate(cos(tt), 1.0f, 0.0f);
	Matrix rotation = RotateY(tt * 100.0f * DEG2RAD);
	Matrix scale = MatrixIdentity();//Scale(cos(tt) * 0.4f + 0.6f, sin(tt) * 0.4f + 0.6f, 1.0f);
	Matrix model = scale * rotation * translation;

	Matrix view = LookAt({ 0.0f, 0.0f, 5.0f }, V3_ZERO, V3_UP);
	//Matrix proj = Perspective(75.0f * DEG2RAD, 1.0f, 0.001f, 100.0f);
	Matrix proj = Ortho(-1.0f, 1.0f, -1.0f, 1.0f, -10.0f, 10.0f);
	// Pay attention to near/far & view z (depth test might fail)!
	
	Matrix mvp = model * view * proj;
	Matrix normal = Transpose(Invert(scale * rotation));
	// Setting w = 0.0 doesn't work in this math library.
	// Must completely remove translation from normal matrix.

	DrawMesh(&mImage, mMesh, mvp, model, normal);
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
