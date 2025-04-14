#include "GraphicsTestScene.h"
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

static int fMeshIndex = 0;
static int fProjIndex = 1;
static Matrix fProjections[] =
{
	Ortho(-10.0f * SCREEN_ASPECT, 10.0f * SCREEN_ASPECT, -10.0f, 10.0f, 0.1f, 100.0f),
	Perspective(90.0f * DEG2RAD, SCREEN_ASPECT, 0.1f, 100.0f)
};

static Texture2D fTexHead;

void DrawDebugShapes();

void GraphicsTestScene::OnLoad()
{
	gCamera = FromView(LookAt({ 0.0f, 0.0f, 5.0f }, V3_ZERO, V3_UP));
	{
		int w, h, c;
		uint8_t* pixels = LoadImage2D("./assets/textures/african_head_diffuse.png", &w, &h, &c, 4, FLIP_VERTICAL);
		CreateTexture2D(&fTexHead, w, h, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, GL_LINEAR, pixels);
		UnloadImage(pixels);
	}
}

void GraphicsTestScene::OnUnload()
{
	DestroyTexture2D(&fTexHead);
}

void GraphicsTestScene::OnUpdate(float dt)
{
	UpdateFpsCameraDefault(gCamera, dt);
	gView = ToView(gCamera);
	gProj = fProjections[fProjIndex];
}

void GraphicsTestScene::OnDraw()
{
	DrawDebugShapes();
}

void GraphicsTestScene::OnDrawImGui()
{
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
