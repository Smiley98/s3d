#include "MainScene.h"
#include "Shader.h"
#include "App.h"
#include "Rasterization.h"
#include "ImageUtils.h"

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
	float nsin = sinf(tt) * 0.5f + 0.5f;
	float ncos = cosf(tt) * 0.5f + 0.5f;

	Matrix translation = Translate(cos(tt), 0.0f, 0.0f);
	Matrix rotation = RotateY(tt * 100.0f * DEG2RAD);
	Matrix scale = Scale(cos(tt) * 0.4f + 0.6f, sin(tt) * 0.4f + 0.6f, 1.0f);

	Matrix model = scale * rotation * translation;
	Matrix view = LookAt({ 0.0f, 0.0f, 5.0f }, V3_ZERO, V3_UP);
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

	data.lightColor = Palette(a, b, c, d, cosf(tt) * 0.5f + 0.5f);
	data.ambient = 0.25f;
	data.diffuse = 0.75f;
	data.specular = 32.0f;

	// This doesn't seem to work...
	// Regardless of rasterization vs wireframes, the object still renders when behind the camera
	// This is probably due to incorrect clipping & no perspective attribute interpolation...
	//Mesh sphere;
	//CopyMesh(gMeshSphere, &sphere);
	//
	//for (size_t i = 0; i < sphere.vertexCount; i++)
	//	sphere.positions[i] = Clip(sphere.positions[i], lt * view * proj);
	//
	//for (size_t i = 0; i < sphere.faceCount; i++)
	//	DrawFaceWireframes(&mImage, sphere.positions, i, Float3ToColor(&data.lightColor.x));
	//
	//DestroyMesh(&sphere);

	DrawMesh(&mImage, mMesh, data);
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
