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

static Vector3 fColor = V3_ONE;
static Vector3 fPosition = V3_ZERO;

static Matrix fWorld;
static Mesh fMesh;

static int fMeshIndex = 0;
static int fProjIndex = 1;
static Matrix fProjections[] =
{
	Ortho(-10.0f * SCREEN_ASPECT, 10.0f * SCREEN_ASPECT, -10.0f, 10.0f, 0.1f, 100.0f),
	Perspective(90.0f * DEG2RAD, SCREEN_ASPECT, 0.1f, 100.0f)
};

static Texture2D fTexHead;
static Cubemap fSkyboxArctic;

void RasterizationScene::OnLoad()
{
	gCamera = FromView(LookAt({ 0.0f, 0.0f, 5.0f }, V3_ZERO, V3_UP));
	GenMeshPar(&fMesh, (MeshType)fMeshIndex);
	CreateMesh(&fMesh);

	{
		int w, h, c;
		uint8_t* pixels[6];
		LoadCubemap("./assets/textures/arctic", "jpg", &w, &h, &c, pixels);
		CreateCubemap(&fSkyboxArctic, w, h, GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE, GL_LINEAR, (void**)pixels);
		UnloadCubemap(pixels);
	}

	{
		int w, h, c;
		uint8_t* pixels = LoadImage2D("./assets/textures/african_head_diffuse.png", &w, &h, &c);
		CreateTexture2D(&fTexHead, w, h, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, GL_LINEAR, pixels);
		UnloadImage(pixels);
	}
}

void RasterizationScene::OnUnload()
{
	DestroyTexture2D(&fTexHead);
	DestroyCubemap(&fSkyboxArctic);
	DestroyMesh(&fMesh);
}

void RasterizationScene::OnUpdate(float dt)
{
	float tt = TotalTime();
	UpdateFpsCameraDefault(gCamera, dt);

	Matrix translation = Translate(fPosition) * (fTranslate ? Translate(cosf(tt), 0.0f, 0.0f) : MatrixIdentity());
	Matrix rotation = fRotate ? RotateY(tt * 100.0f * DEG2RAD) : MatrixIdentity();
	Matrix scale = fScale ? Scale(cosf(tt), sinf(tt), 1.0f) : MatrixIdentity();

	fWorld = scale * rotation * translation;
	gView = ToView(gCamera);
	gProj = fProjections[fProjIndex];
}

void RasterizationScene::OnDraw()
{
	DrawMeshDebug(fMesh, fWorld, fColor);
	DrawMeshTexture(gMeshHead, Translate(0.0f, 0.0f, -5.0f), fTexHead, 0);
	DrawMeshReflect(gMeshCube, Translate(-2.0f, 0.0f, 0.0f), fSkyboxArctic, 0);
	DrawMeshRefract(gMeshCube, Translate(2.0f, 0.0f, 0.0f), fSkyboxArctic, 0, 1.00f / 1.52f); // glass
	DrawSkybox(fSkyboxArctic, 0);
}

void RasterizationScene::OnDrawImGui()
{
	static const char* meshNames[] =
	{
		"Triangle",
		"Square",
		"Hexagon",
		"Circle",
		"Semicircle",

		"Cube",
		"Sphere",
		"Hemisphere",
		"Cylinder",

		"Plane_Z",
		"Plane_Y",
		"Plane_X"
	};

	ImGui::RadioButton("Orthographic", &fProjIndex, 0); ImGui::SameLine();
	ImGui::RadioButton("Perspective", &fProjIndex, 1);

	ImGui::Checkbox("Translate", &fTranslate); ImGui::SameLine();
	ImGui::Checkbox("Rotate", &fRotate); ImGui::SameLine();
	ImGui::Checkbox("Scale", &fScale);

	ImGui::SliderFloat3("Object Position", &fPosition.x, -10.0f, 10.0f);

	bool gen = ImGui::Combo("Meshes", &fMeshIndex, meshNames, IM_ARRAYSIZE(meshNames));
	if (gen)
	{
		DestroyMesh(&fMesh);
		GenMeshPar(&fMesh, (MeshType)fMeshIndex);
		CreateMesh(&fMesh);
	}

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
	ImGui::Combo("Shaders", &shaderIndex, shaderNames, IM_ARRAYSIZE(shaderNames));
	gDebugShader = (DebugShaderType)shaderIndex;
	
	ImGui::ColorPicker3("Colour", &fColor.x);
	//ImGui::ShowDemoWindow();
}
