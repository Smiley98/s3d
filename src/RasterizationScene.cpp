#include "RasterizationScene.h"
#include "Rasterization.h"
#include "ImageUtils.h"
#include "Camera.h"
#include "Time.h"

static Image fDiffuseMap;

void RasterizationScene::OnLoad()
{
	gCamera = FromView(LookAt({ 0.0f, 0.0f, 5.0f }, V3_ZERO, V3_UP));
	CreateImageFromFile(&fDiffuseMap, "assets/textures/african_head_diffuse.png", true);
}

void RasterizationScene::OnUnload()
{
	DestroyImage(&fDiffuseMap);
}

void RasterizationScene::OnUpdate(float dt)
{
	ClearColor(&gImageCPU, BLACK);
	ClearDepth(&gImageCPU, 1.0f);

	float tt = TotalTime();
	float nsin = sinf(tt) * 0.5f + 0.5f;
	float ncos = cosf(tt) * 0.5f + 0.5f;

	Matrix translation = MatrixIdentity();//Translate(cos(tt), 0.0f, 0.0f);
	Matrix rotation = MatrixIdentity();//RotateY(tt * 100.0f * DEG2RAD);
	Matrix scale = MatrixIdentity();//Scale(cos(tt) * 0.4f + 0.6f, sin(tt) * 0.4f + 0.6f, 1.0f);
	UpdateFpsCameraDefault(gCamera, dt);

	Matrix model = scale * rotation * translation;
	Matrix view = ToView(gCamera);
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

	DrawMesh(&gImageCPU, gMeshHead, data, &fDiffuseMap);
}
