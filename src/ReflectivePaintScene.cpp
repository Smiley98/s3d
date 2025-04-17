#include "ReflectivePaintScene.h"
#include "Window.h"
#include "Time.h"
#include "Render.h"
#include "Camera.h"
#include "ImageLoader.h"
#include <imgui/imgui.h>

static Vector3 fLightDirection = { 1.0f, -1.0f, -1.0f };

static float fAmbient = 0.25f;
static float fDiffuse = 0.75f;
static float fSpecular = 1.0f;

static TextureCubemap fSkyboxArctic;
static TextureCubemap fSkyboxSpace;

static TextureCubemap fGradientHot;
static TextureCubemap fGradientCold;

static Texture2D fTextureDiffuse;
static Texture2D fTextureSpecular;

struct Car
{
	TextureCubemap paint;
	float paintIntensity;
	float environmentIntensity;
};

static Car fCars[3];
static int fCar = 1;
static int fSkybox = 0;
static bool fDrive = false;

void GenGradientCubemap(TextureCubemap* map, Vector3 TL, Vector3 TR, Vector3 BL, Vector3 BR);
void DrawCar(const Car& car, Matrix world);

void ReflectivePaintScene::OnLoad()
{
	gCamera = FromView(LookAt({ 0.0f, 15.0f, 30.0f }, V3_ZERO, V3_UP));

	{
		int w, h, c;
		uint8_t* pixels = LoadImage2D("./assets/textures/ct4_grey.png", &w, &h, &c, 4, FLIP_VERTICAL);
		CreateTexture2D(&fTextureDiffuse, w, h, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, GL_LINEAR, pixels);
		UnloadImage(pixels);
	}

	{
		int w, h, c;
		uint8_t* pixels = LoadImage2D("./assets/textures/ct4_specular.png", &w, &h, &c, 1, FLIP_VERTICAL);
		CreateTexture2D(&fTextureSpecular, w, h, GL_R8, GL_RED, GL_UNSIGNED_BYTE, GL_NEAREST, pixels);
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

	CreateTextureCubemap(&fGradientHot, 512, 512, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, GL_NEAREST);
	CreateTextureCubemap(&fGradientCold, 512, 512, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, GL_NEAREST);

	GenGradientCubemap(&fGradientHot, { 1.0f, 1.0f, 0.0f }, { 0.5f, 1.0f, 0.0f }, { 1.0f, 0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f });
	GenGradientCubemap(&fGradientCold, { 0.5f, 0.0f, 0.5f }, { 0.0f, 0.0f, 0.5f }, { 0.25f, 0.0f, 0.25f }, { 0.0f, 0.5f, 0.0f });

	fCars[0].paint = fGradientHot;
	fCars[2].paint = fGradientCold;
	fCars[1].paint = fSkyboxSpace;

	fCars[0].paintIntensity = 1.75f;
	fCars[2].paintIntensity = 1.75f;
	fCars[1].paintIntensity = 2.0f;

	fCars[0].environmentIntensity = 1.25f;
	fCars[2].environmentIntensity = 1.25f;
	fCars[1].environmentIntensity = 0.25f;
}

void ReflectivePaintScene::OnUnload()
{
	DestroyTextureCubemap(&fGradientCold);
	DestroyTextureCubemap(&fGradientHot);

	DestroyTextureCubemap(&fSkyboxSpace);
	DestroyTextureCubemap(&fSkyboxArctic);

	DestroyTexture2D(&fTextureSpecular);
	DestroyTexture2D(&fTextureDiffuse);
}

void ReflectivePaintScene::OnUpdate(float dt)
{
	UpdateFpsCameraDefault(gCamera, dt);
	gView = ToView(gCamera);
	gProj = Perspective(90.0f * DEG2RAD, SCREEN_ASPECT, 0.1f, 100.0f);
}

void ReflectivePaintScene::OnDraw()
{
	Matrix r = RotateY(-50.0f * TotalTime() * DEG2RAD);
	for (int i = 0; i < 3; i++)
	{
		Matrix world = RotateY(-2.0f * PI * 0.333f * i);
		world = world * Translate(Right(world) * (20.0f + i * 10.0f)) * r;
		world = fDrive ? world : Translate(V3_RIGHT * (-20.0f + i * 20.0f));
		DrawCar(fCars[i], world);
	}

	static TextureCubemap skyboxes[]
	{
		fSkyboxArctic,
		fSkyboxSpace,
		fGradientHot,
		fGradientCold,
	};

	DrawSkybox(skyboxes[fSkybox], 0);
}

void ReflectivePaintScene::OnDrawImGui()
{
	ImGui::SliderFloat3("Light Direction", &fLightDirection.x, -1.0f, 1.0f);
	ImGui::SliderFloat("Ambient", &fAmbient, 0.0f, 2.0f);
	ImGui::SliderFloat("Diffuse", &fDiffuse, 0.0f, 1.0f);
	ImGui::SliderFloat("Specular", &fSpecular, 0.0f, 1.0f);

	// Just for fun. Cars always reflect the arctic skybox since the other 3 cubemaps are used for car paint.
	ImGui::RadioButton("Artic Skybox", &fSkybox, 0); ImGui::SameLine();
	ImGui::RadioButton("Galaxy Skybox", &fSkybox, 1); ImGui::SameLine();
	ImGui::RadioButton("Warm Skybox", &fSkybox, 2); ImGui::SameLine();
	ImGui::RadioButton("Cold Skybox", &fSkybox, 3);

	static bool carSelect = false;
	ImGui::Checkbox("Car Select", &carSelect); ImGui::SameLine();
	ImGui::Checkbox("Drive", &fDrive); 

	if (carSelect)
	{
		ImGui::RadioButton("Warm car", &fCar, 0); ImGui::SameLine();
		ImGui::RadioButton("Galaxy car", &fCar, 1); ImGui::SameLine();
		ImGui::RadioButton("Cold car", &fCar, 2);

		Car& car = fCars[fCar];
		ImGui::SliderFloat("Paint Intensity", &car.paintIntensity, 0.0f, 2.0f);
		ImGui::SliderFloat("Environment Intensity", &car.environmentIntensity, 0.0f, 2.0f);
	}

}

void DrawCar(const Car& car, Matrix world)
{
	Matrix mvp = world * gView * gProj;

	BindTexture2D(fTextureDiffuse, 0);
	BindTexture2D(fTextureSpecular, 1);
	BindTextureCubemap(car.paint, 2);
	BindTextureCubemap(fSkyboxArctic, 3);
	BindShader(&gShaderCars);

	SendMat4("u_mvp", mvp);
	SendMat4("u_world", world);
	SendMat3("u_normal", NormalMatrix(world));
	SendVec3("u_camPos", gCamera.position);

	SendFloat("u_paintIntensity", car.paintIntensity);
	SendFloat("u_environmentIntensity", car.environmentIntensity);

	SendVec3("u_lightDirection", Normalize(fLightDirection));

	SendFloat("u_ambient", fAmbient);
	SendFloat("u_diffuse", fDiffuse);
	SendFloat("u_specular", fSpecular);

	SendInt("u_paintMask", 0);
	SendInt("u_specularMask", 1);
	SendInt("u_paintColor", 2);
	SendInt("u_environmentColor", 3);

	DrawMesh(gMeshCt4);

	UnbindShader();
	UnbindTextureCubemap(fSkyboxArctic, 3);
	UnbindTextureCubemap(car.paint, 2);
	UnbindTexture2D(fTextureSpecular, 1);
	UnbindTexture2D(fTextureDiffuse, 0);
}

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

// Giving each car a spotlight would be cool, but in order to look nice the scene would need an interior.
// I'd also need to support transparency to support the cone-shaped beam.
// Ultimately, p2 doesn't need this. Add spline-following and move on!
