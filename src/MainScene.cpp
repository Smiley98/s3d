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
	mMesh2 = gMeshSphere;
	mColors.resize(mMesh.vertexCount / 3);
	for (size_t i = 0; i < mColors.size(); i++)
		mColors[i] = RandRGB();
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

	for (size_t i = 0, c = 0; i < mMesh.vertexCount; i += 3, c++)
		DrawFaceFront(&mImage, mMesh, i);

	float tt = TotalTime();
	Vector3 v = V3_ONE;

	Mesh copy;
	copy.vertexCount = mMesh2.vertexCount;
	copy.positions = new Vector3[copy.vertexCount];
	for (size_t i = 0; i < copy.vertexCount; i++)
	{
		Matrix transform = Translate(0.0f, 0.0f, cosf(tt));
		copy.positions[i] = Multiply(mMesh2.positions[i], transform);
	}

	for (size_t i = 0, c = 0; i < copy.vertexCount; i += 3, c++)
		DrawFaceFront(&mImage, copy, i);

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

void MainScene::OnDrawGui()
{
}

void MainScene::AnimationTest()
{
	ClearColor(&mImage, BLACK);
	float tt = TotalTime();
	float ncos = cosf(tt) * 0.25f + 0.75f;
	Vector3 v = V3_ONE;
	v *= ncos;

	Mesh copy;
	copy.vertexCount = mMesh.vertexCount;
	copy.positions = new Vector3[copy.vertexCount];
	for (size_t i = 0; i < mMesh.vertexCount; i++)
	{
		Matrix transform =
			Scale(v.x, v.y, v.z) *
			RotateZ(TotalTime() * 100.0f * DEG2RAD) *
			Translate(cosf(tt), 0.0f, 0.0f);
		copy.positions[i] = Multiply(mMesh.positions[i], transform);
	}

	for (size_t i = 0, c = 0; i < copy.vertexCount; i += 3, c++)
		DrawFaceFront(&mImage, copy, i);

	for (size_t i = 0, c = 0; i < copy.vertexCount; i += 3, c++)
		DrawFaceWireframes(&mImage, copy, i, mColors[c]);

	delete[] copy.positions;
}
