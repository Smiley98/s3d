#include "RasterizationScene.h"
#include "Window.h"
#include "Time.h"
#include "Image.h"
#include "Camera.h"
#include <imgui/imgui.h>

bool fTranslate = false;
bool fRotate = false;
bool fScale = false;

Mesh fMesh;
Texture fTexture;
Vector3 fColor = V3_ONE;
Vector3 fPosition = V3_ZERO;

Camera fCamera;

void RasterizationScene::OnCreate()
{
	CreateTextureFromImage(&fTexture, gImageDiffuse);
	GenEquilateral(fMesh);

	fCamera.position = { 0.0f, 0.0f, 5.0f };
	SetMousePosition({ SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f });
}

void RasterizationScene::OnDestroy()
{
	DestroyTexture(&fTexture);
	DestroyMesh(fMesh);
}

void RasterizationScene::OnDraw()
{
	float tt = TotalTime();
	float dt = FrameTime();

	UpdateCameraAuto(fCamera, dt);
	gView = fCamera.view;

	Matrix translation = Translate(fPosition) *
		(fTranslate ? Translate(cosf(tt), 0.0f, 0.0f) : MatrixIdentity());
	
	Matrix rotation =
		fRotate ? RotateY(tt * 100.0f * DEG2RAD) : MatrixIdentity();
	
	Matrix scale =
		fScale ? Scale(cosf(tt), sinf(tt), 1.0f) : MatrixIdentity();

	Matrix world = scale * rotation * translation;
	Matrix mvp = world * gView * gProj;

	// Render object
	DrawMeshDebug(fMesh, mvp, world, fColor);

	// Successful texture render!
	//DrawMeshTexture(gMeshHead, mvp, world, fTexture);

	// Render 1x1 quad for reference
	//DebugShaderType type = gDebugShader;
	//gDebugShader = WIRE;
	//SetDepthTest(false);
	//SetWireframes(true);
	//DrawPlaneXY({}, 1.0f, 1.0f, V3_ONE);
	//SetWireframes(false);
	//SetDepthTest(true);
	//gDebugShader = type;
}

void GenHead(Mesh& mesh)
{
	CopyMesh(gMeshHead, mesh);
}

void RasterizationScene::OnDrawImGui()
{
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

		"Plane_XZ",
		"Plane_YZ",
		"Plane_XY",
		"Dodecahedron",

		"Head"
	};

	using MeshGenerator = void(*)(Mesh&);
	static MeshGenerator generators[] =
	{
		GenEquilateral,
		GenSquare,
		GenCircle,
		GenSemicircle,

		GenCube,
		GenSphere,
		GenHemisphere,
		GenCylinder,

		GenPlaneXZ,
		GenPlaneYZ,
		GenPlaneXY,
		GenDodecahedron,

		GenHead
	};

	static Matrix projections[] =
	{
		Ortho(-1.0f * SCREEN_ASPECT, 1.0f * SCREEN_ASPECT, -1.0f, 1.0f, 0.01f, 10.0f),
		Perspective(75.0f * DEG2RAD, 1.0f, 0.001f, 100.0f)
	};

	static int projIndex = 1;
	ImGui::RadioButton("Orthographic", &projIndex, 0); ImGui::SameLine();
	ImGui::RadioButton("Perspective", &projIndex, 1);
	gProj = projections[projIndex];

	ImGui::Checkbox("Translate", &fTranslate); ImGui::SameLine();
	ImGui::Checkbox("Rotate", &fRotate); ImGui::SameLine();
	ImGui::Checkbox("Scale", &fScale);

	ImGui::SliderFloat3("Object Position", &fPosition.x, -10.0f, 10.0f);

	// Legacy look-at & fps gui controls. Now using mouse delta for camera input!
	//static Vector3 camPos{ 0.0f, 0.0f, 5.0f };
	//static float pitch = 0.0f;
	//static float yaw = 0.0f;
	//ImGui::SliderFloat3("Camera Position", &camPos.x, -10.0f, 10.0f);
	//ImGui::SliderAngle("Pitch", &pitch);
	//ImGui::SliderAngle("Yaw", &yaw);
	//fView = LookAtFps(camPos, FromEuler(pitch, yaw, 0.0f));
	//fView = LookAt(camPos, V3_ZERO, V3_UP);

	static int meshIndex = 0;
	bool gen = ImGui::Combo("Meshes", &meshIndex, meshNames, IM_ARRAYSIZE(meshNames));
	if (gen)
	{
		DestroyMesh(fMesh);
		generators[meshIndex](fMesh);
	}

	static const char* shaderNames[] =
	{
		"Flat",
		"Wireframe",
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
