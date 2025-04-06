#include "RasterizationScene.h"
#include "Window.h"
#include "Time.h"
#include "Render.h"
#include "Camera.h"
#include "ImageLoader.h"
#include <imgui/imgui.h>

static bool fTranslate = false;
static bool fRotate = false;
static bool fScale = false;

static Vector3 fLightPosition = { -15.0f, 15.0f, 0.0f };
static Vector3 fColor = V3_ONE;

static float fPaintIntensity = 1.5f;
static float fEnvironmentIntensity = 1.25f;

static float fAmbient = 1.0f;
static float fDiffuse = 0.0f;
static float fSpecular = 1.0f;

static int fMeshIndex = 0;
static int fProjIndex = 1;
static Matrix fProjections[] =
{
	Ortho(-10.0f * SCREEN_ASPECT, 10.0f * SCREEN_ASPECT, -10.0f, 10.0f, 0.1f, 100.0f),
	Perspective(90.0f * DEG2RAD, SCREEN_ASPECT, 0.1f, 100.0f)
};

static Texture2D fTexHead;
static TextureCubemap fSkyboxArctic;
static TextureCubemap fSkyboxSpace;

static Texture2D fTexPaint;
static Texture2D fTexSpecular;

void DrawDebugShapes();

void RasterizationScene::OnLoad()
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
		uint8_t* pixels = LoadImage2D("./assets/textures/african_head_diffuse.png", &w, &h, &c, 4, FLIP_VERTICAL);
		CreateTexture2D(&fTexHead, w, h, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, GL_LINEAR, pixels);
		UnloadImage(pixels);
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

void RasterizationScene::OnUnload()
{
	DestroyTexture2D(&fTexSpecular);
	DestroyTexture2D(&fTexPaint);
	DestroyTexture2D(&fTexHead);
	DestroyTextureCubemap(&fSkyboxSpace);
	DestroyTextureCubemap(&fSkyboxArctic);
}

void RasterizationScene::OnUpdate(float dt)
{
	UpdateFpsCameraDefault(gCamera, dt);
	gView = ToView(gCamera);
	gProj = fProjections[fProjIndex];
}

void RasterizationScene::OnDraw()
{
	float tt = TotalTime();
	float lightRadius = 50.0f;

	//TextureCubemap paint = fSkyboxSpace;
	TextureCubemap paint = GetChameleonMap();
	
	float angle = fRotate ? tt * 100.0f * DEG2RAD : 0.0f;
	Matrix world = RotateY(angle);
	Matrix mvp = world * gView * gProj;

	BindTextureCubemap(paint, 0);
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
	UnbindTextureCubemap(paint, 0);

	DrawSkybox(fSkyboxArctic, 0);

	DrawMeshWireframes(gMeshSphere, Scale(V3_ONE * lightRadius) * Translate(fLightPosition), V3_RIGHT);
	//DrawDebugShapes();
}

void RasterizationScene::OnDrawImGui()
{
	ImGui::SliderFloat3("Light Position", &fLightPosition.x, -50.0f, 50.0f);

	ImGui::SliderFloat("Paint Intensity", &fPaintIntensity, 0.0f, 2.0f);
	ImGui::SliderFloat("Environment Intensity", &fEnvironmentIntensity, 0.0f, 2.0f);

	ImGui::SliderFloat("Ambient", &fAmbient, 0.0f, 2.0f);
	ImGui::SliderFloat("Diffuse", &fDiffuse, 0.0f, 1.0f);
	ImGui::SliderFloat("Specular", &fSpecular, 0.0f, 1.0f);

	ImGui::RadioButton("Orthographic", &fProjIndex, 0); ImGui::SameLine();
	ImGui::RadioButton("Perspective", &fProjIndex, 1);

	ImGui::Checkbox("Translate", &fTranslate); ImGui::SameLine();
	ImGui::Checkbox("Rotate", &fRotate); ImGui::SameLine();
	ImGui::Checkbox("Scale", &fScale);

	static const char* meshNames[] =
	{
		"Triangle",
		"Square",
		"Circle",
		"Semicircle",

		"Cube",
		"Sphere",
		"Hemisphere",
		"Cylinder",

		"Head",
	};

	static const char* shaderNames[] =
	{
		"Flat",
		"Wireframe",
		"Depth",
		"World-Space Positions",
		"Screen-Space Positions",
		"Object-Space Normals",
		"World-Space Normals",
		"Texture Coordinates"
	};

	static int shaderIndex = 0;
	ImGui::Combo("Meshes", &fMeshIndex, meshNames, IM_ARRAYSIZE(meshNames));
	ImGui::Combo("Shaders", &shaderIndex, shaderNames, IM_ARRAYSIZE(shaderNames));
	gDebugShader = (DebugShaderType)shaderIndex;
	
	ImGui::ColorPicker3("Colour", &fColor.x);
	//ImGui::ShowDemoWindow();
}

void DrawDebugShapes()
{
	float tt = TotalTime();
	Vector3 position = fTranslate ? Vector3{ cosf(tt), 0.0f, 0.0f } : V3_ZERO;
	float angle = fRotate ? tt * 100.0f * DEG2RAD : 0.0f;
	Vector3 size = fScale ? Vector3{ cosf(tt), sinf(tt), 1.0f } : V3_ONE;

	Matrix translation = Translate(position);
	Matrix rotation = RotateY(angle);
	Matrix scale = Scale(size);
	Matrix world = scale * rotation * translation;

	switch (fMeshIndex)
	{
	case 0:
	{
		Vector3 v0{ 0.0f, 1.0f, 0.0f };
		Vector3 v1{ -1.0f * sinf(PI / 3.0f), -1.0f * cosf(PI / 3.0f), 0.0f };
		Vector3 v2{ 1.0f * sinf(PI / 3.0f), -1.0f * cosf(PI / 3.0f), 0.0f };
		DrawTriangle(v0, v1, v2, fColor, angle);
	}
	break;

	case 1:
		DrawRectangle(position, size.x, size.y, fColor, angle);
		break;

	case 2:
		DrawCircle(position, size.x, fColor, angle);
		break;

	case 3:
		DrawSemicircleY(position, size.x, fColor, angle);
		break;

	case 4:
		DrawCube(position, size.x, size.y, size.z, fColor, rotation);
		break;

	case 5:
		DrawSphere(position, size.x, fColor, rotation);
		break;

	case 6:
		DrawHemisphere(position, size.x, fColor, rotation);
		break;

	case 7:
		DrawCylinder(position, size.x, size.y, fColor, rotation);
		break;

	case 8:
		DrawMeshDebug(gMeshHead, world, fColor);
		break;

	default:
		assert(false, "Invalid Mesh");
	}
}
