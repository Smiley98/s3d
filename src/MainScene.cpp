#include "MainScene.h"
#include "Time.h"
#include "Render.h"
#include "Rasterization.h"
#include "ImageUtils.h"
#include "Camera.h"

constexpr int IMAGE_SIZE = 512;

Image fDiffuseMap;

void MainScene::OnLoad()
{
	CreateImageFromMemory(&mImage, IMAGE_SIZE, IMAGE_SIZE);
	CreateTextureFromMemory(&mTexture, IMAGE_SIZE, IMAGE_SIZE, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, GL_NEAREST);
	CreateImageFromFile(&fDiffuseMap, "assets/textures/african_head_diffuse.png", true);
	mMesh = gMeshHead;
	gCamera.position = { 0.0f, 0.0f, 5.0f };
}

void MainScene::OnUnload()
{
	DestroyImage(&fDiffuseMap);
	DestroyTexture(&mTexture);
	DestroyImage(&mImage);
}

void MainScene::OnUpdate(float dt)
{
	ClearColor(&mImage, BLACK);
	ClearDepth(&mImage, 1.0f);

	float tt = TotalTime();
	float nsin = sinf(tt) * 0.5f + 0.5f;
	float ncos = cosf(tt) * 0.5f + 0.5f;

	Matrix translation = MatrixIdentity();//Translate(cos(tt), 0.0f, 0.0f);
	Matrix rotation = RotateY(tt * 100.0f * DEG2RAD);
	Matrix scale = MatrixIdentity();//Scale(cos(tt) * 0.4f + 0.6f, sin(tt) * 0.4f + 0.6f, 1.0f);
	UpdateFpsCameraDefault(gCamera, dt);

	Matrix model = scale * rotation * translation;
	Matrix view = ToView(gCamera);
	//Matrix view = LookAt({ 0.0f, 0.0f, 5.0f }, V3_ZERO, V3_UP);
	Matrix proj = Perspective(75.0f * DEG2RAD, 1.0f, 0.001f, 100.0f);
	Matrix mvp = model * view * proj;

	Matrix lt = Translate(5.0f, sin(tt * 3.0f) * 5.0f, 0.0f) * RotateY(tt * 100.0f * DEG2RAD);

	UniformData data;
	data.mvp = mvp;
	data.world = model;
	data.cameraPosition = { 0.0f, 0.0f, 5.0f };
	data.lightPosition = lt * V3_ZERO;

	// Palette parameters
	Vector3 a{ 0.5f, 0.5f, 0.5f };
	Vector3 b{ 0.5f, 0.5f, 0.5f };
	Vector3 c{ 1.0f, 1.0f, 1.0f };
	Vector3 d{ 0.263f, 0.416f, 0.557f };

	data.lightColor = V3_ONE;//Palette(a, b, c, d, cosf(tt) * 0.5f + 0.5f);
	data.ambient = 0.25f;
	data.diffuse = 0.75f;
	data.specular = 32.0f;

	DrawMesh(&mImage, mMesh, data, &fDiffuseMap);
}

void MainScene::OnDraw() 
{
	UpdateTexture(mTexture, (unsigned char*)mImage.pixels.data());
	DrawFsqTexture(mTexture);
}
