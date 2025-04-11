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

static TextureCubemap fSkyboxHot;
static TextureCubemap fSkyboxCold;

static Texture2D fTexDiffuse;
static Texture2D fTexSpecular;

void GenGradientCubemap(TextureCubemap* map, Vector3 TL, Vector3 TR, Vector3 BL, Vector3 BR)
{
	GLuint fb = GL_NONE;
	glGenFramebuffers(1, &fb);
	glBindFramebuffer(GL_FRAMEBUFFER, fb);
	BindShader(&gShaderGradientCubemap);

	for (int i = 0; i < 6; i++)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, map->id, 0);
		assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
		glViewport(0, 0, 512.0f, 512.0f);
		SendVec2("u_res", { 512.0f, 512.0f });
		SendInt("u_face", i);
		SendVec3("u_TL", TL);
		SendVec3("u_TR", TR);
		SendVec3("u_BL", BL);
		SendVec3("u_BR", BR);
		DrawFsq();
	}

	UnbindShader();
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
	glDeleteFramebuffers(1, &fb);
}

void CarScene::OnLoad()
{
	gCamera = FromView(LookAt({ 0.0f, 0.0f, 5.0f }, V3_ZERO, V3_UP));

	{
		int w, h, c;
		uint8_t* pixels = LoadImage2D("./assets/textures/ct4_grey.png", &w, &h, &c, 4, FLIP_VERTICAL);
		CreateTexture2D(&fTexDiffuse, w, h, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, GL_LINEAR, pixels);
		UnloadImage(pixels);
	}

	{
		int w, h, c;
		uint8_t* pixels = LoadImage2D("./assets/textures/ct4_specular.png", &w, &h, &c, 1, FLIP_VERTICAL);
		CreateTexture2D(&fTexSpecular, w, h, GL_R8, GL_RED, GL_UNSIGNED_BYTE, GL_NEAREST, pixels);
		UnloadImage(pixels);
	}

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

	CreateTextureCubemap(&fSkyboxHot, 512, 512, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, GL_NEAREST);
	CreateTextureCubemap(&fSkyboxCold, 512, 512, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, GL_NEAREST);

	// TODO - warm car on right, cold car on left, galaxy car following spline in figure-8
	GenGradientCubemap(&fSkyboxCold, { 0.5f, 0.0f, 0.5f }, { 0.0f, 0.0f, 0.5f }, { 0.25f, 0.0f, 0.25f }, { 0.0f, 0.5f, 0.0f });
	GenGradientCubemap(&fSkyboxHot, { 1.0f, 1.0f, 0.0f }, { 0.5f, 1.0f, 0.0f }, { 1.0f, 0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f });
}

void CarScene::OnUnload()
{
	DestroyTextureCubemap(&fSkyboxCold);
	DestroyTextureCubemap(&fSkyboxHot);

	DestroyTextureCubemap(&fSkyboxSpace);
	DestroyTextureCubemap(&fSkyboxArctic);

	DestroyTexture2D(&fTexSpecular);
	DestroyTexture2D(&fTexDiffuse);
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

	TextureCubemap paint = fSkyboxHot;
	TextureCubemap environment = fSkyboxArctic;

	Matrix world = MatrixIdentity();
	Matrix mvp = world * gView * gProj;

	BindTextureCubemap(paint, 0);
	BindTextureCubemap(environment, 1);

	BindTexture2D(fTexDiffuse, 0);
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
	UnbindTexture2D(fTexDiffuse, 0);
	UnbindTextureCubemap(environment, 1);
	UnbindTextureCubemap(paint, 0);

	DrawSkybox(environment, 0);

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
