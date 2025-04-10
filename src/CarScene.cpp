#include "CarScene.h"
#include "Window.h"
#include "Time.h"
#include "Render.h"
#include "Camera.h"
#include "ImageLoader.h"
#include <imgui/imgui.h>

static Vector3 fLightPosition = { -15.0f, 15.0f, 0.0f };
static Vector3 fColor = V3_ONE;

static float fPaintIntensity = 1.5f;
static float fEnvironmentIntensity = 1.25f;

static float fAmbient = 1.0f;
static float fDiffuse = 0.0f;
static float fSpecular = 1.0f;

static TextureCubemap fSkyboxArctic;
static TextureCubemap fSkyboxSpace;

static Texture2D fTexPaint;
static Texture2D fTexSpecular;

void CarScene::OnLoad()
{
	gCamera = FromView(LookAt({ 0.0f, 0.0f, 5.0f }, V3_ZERO, V3_UP));

	{
		int w, h, c;
		uint8_t* pixels[6];
		LoadImageCubemap("./assets/textures/arctic", "jpg", &w, &h, &c, 4, pixels);
		CreateTextureCubemap(&fSkyboxArctic, w, h, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, GL_LINEAR, (void**)pixels);
		UnloadImageCubemap(pixels);
	}

	{
		int w, h, c;
		uint8_t* pixels[6];
		LoadImageCubemap("./assets/textures/space", "png", &w, &h, &c, 4, pixels);
		CreateTextureCubemap(&fSkyboxSpace, w, h, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, GL_LINEAR, (void**)pixels);
		UnloadImageCubemap(pixels);
	}

	{
		int w, h, c;
		uint8_t* pixels = LoadImage2D("./assets/textures/ct4_grey.png", &w, &h, &c, 4, FLIP_VERTICAL);
		CreateTexture2D(&fTexPaint, w, h, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, GL_LINEAR, pixels);
		UnloadImage(pixels);
	}

	{
		int w, h, c;
		uint8_t* pixels = LoadImage2D("./assets/textures/ct4_specular.png", &w, &h, &c, 1, FLIP_VERTICAL);
		CreateTexture2D(&fTexSpecular, w, h, GL_R8, GL_RED, GL_UNSIGNED_BYTE, GL_NEAREST, pixels);
		UnloadImage(pixels);
	}
}

void CarScene::OnUnload()
{
	DestroyTexture2D(&fTexSpecular);
	DestroyTexture2D(&fTexPaint);
	DestroyTextureCubemap(&fSkyboxSpace);
	DestroyTextureCubemap(&fSkyboxArctic);
}

void CarScene::OnUpdate(float dt)
{
	UpdateFpsCameraDefault(gCamera, dt);
	gView = ToView(gCamera);
	gProj = Perspective(90.0f * DEG2RAD, SCREEN_ASPECT, 0.1f, 100.0f);
}

void CarScene::OnDraw()
{
	float tt = TotalTime();
	float lightRadius = 50.0f;

	//TextureCubemap paint = fSkyboxSpace;
	TextureCubemap paint = GetChameleonMap();
	
	Matrix world = MatrixIdentity();//RotateY(angle);
	Matrix mvp = world * gView * gProj;

	BindTextureCubemap(fSkyboxSpace, 0);
	BindTextureCubemap(fSkyboxArctic, 1);

	BindTexture2D(fTexPaint, 0);
	BindTexture2D(fTexSpecular, 1);

	BindShader(&gShaderPhongEnv);

	SendMat4("u_mvp", mvp);
	SendMat4("u_world", world);
	SendMat3("u_normal", NormalMatrix(world));
	SendVec3("u_camPos", gCamera.position);

	SendFloat("u_paintIntensity", fPaintIntensity);
	SendFloat("u_environmentIntensity", fEnvironmentIntensity);

	SendVec3("u_lightPosition", fLightPosition);
	SendFloat("u_lightRadius", lightRadius);

	SendFloat("u_ambient", fAmbient);
	SendFloat("u_diffuse", fDiffuse);
	SendFloat("u_specular", fSpecular);

	SendInt("u_paintColor", 0);
	SendInt("u_environmentColor", 1);

	SendInt("u_paintMask", 0);
	SendInt("u_specularMask", 1);

	DrawMesh(gMeshCt4);
	UnbindShader();
	UnbindTexture2D(fTexSpecular, 1);
	UnbindTexture2D(fTexPaint, 0);
	UnbindTextureCubemap(fSkyboxArctic, 1);
	UnbindTextureCubemap(fSkyboxSpace, 0);

	DrawSkybox(fSkyboxArctic, 0);

	DrawMeshWireframes(gMeshSphere, Scale(V3_ONE * lightRadius) * Translate(fLightPosition), V3_RIGHT);
}

void CarScene::OnDrawImGui()
{
	ImGui::SliderFloat3("Light Position", &fLightPosition.x, -50.0f, 50.0f);

	ImGui::SliderFloat("Paint Intensity", &fPaintIntensity, 0.0f, 2.0f);
	ImGui::SliderFloat("Environment Intensity", &fEnvironmentIntensity, 0.0f, 2.0f);

	ImGui::SliderFloat("Ambient", &fAmbient, 0.0f, 2.0f);
	ImGui::SliderFloat("Diffuse", &fDiffuse, 0.0f, 1.0f);
	ImGui::SliderFloat("Specular", &fSpecular, 0.0f, 1.0f);

	//ImGui::ShowDemoWindow();
}
