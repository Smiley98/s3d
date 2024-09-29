#include "RasterizationScene.h"
#include "Window.h"
#include "Time.h"
#include "Image.h"
#include "Camera.h"
#include <imgui/imgui.h>

bool fTranslate = false;
bool fRotate = false;
bool fScale = false;

Matrix fWorld;
Matrix fMvp;

Texture fTexture;
Vector3 fColor = V3_ONE;
Vector3 fPosition = V3_ZERO;

Mesh fMesh;
void GenHead(Mesh& mesh);

void RasterizationScene::OnLoad()
{
	GenEquilateral(fMesh);
	CreateTextureFromImage(&fTexture, gImageDiffuse);
	gCamera = gCameraPersp75;
	gProj = gCamera.proj;
}

void RasterizationScene::OnUnload()
{
	DestroyTexture(&fTexture);
	DestroyMesh(fMesh);
}

void RasterizationScene::OnUpdate(float dt)
{
	float tt = TotalTime();
	UpdateCameraDefault(gCamera, dt);
	gView = gCamera.view;
	// TODO -- Make some sort of certralized world-view-projection update

	Matrix translation = Translate(fPosition) *
		(fTranslate ? Translate(cosf(tt), 0.0f, 0.0f) : MatrixIdentity());

	Matrix rotation =
		fRotate ? RotateY(tt * 100.0f * DEG2RAD) : MatrixIdentity();

	Matrix scale =
		fScale ? Scale(cosf(tt), sinf(tt), 1.0f) : MatrixIdentity();

	fWorld = scale * rotation * translation;
	fMvp = fWorld * gView * gProj;
}

void RasterizationScene::OnDraw()
{
	DrawMeshDebug(fMesh, fMvp, fWorld, fColor);
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

	static Camera* projections[] =
	{
		&gCameraOrtho10,
		&gCameraPersp75
	};

	static int projIndex = 1;
	ImGui::RadioButton("Orthographic", &projIndex, 0); ImGui::SameLine();
	ImGui::RadioButton("Perspective", &projIndex, 1);
	gProj = projections[projIndex]->proj;

	ImGui::Checkbox("Translate", &fTranslate); ImGui::SameLine();
	ImGui::Checkbox("Rotate", &fRotate); ImGui::SameLine();
	ImGui::Checkbox("Scale", &fScale);

	ImGui::SliderFloat3("Object Position", &fPosition.x, -10.0f, 10.0f);

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

void GenHead(Mesh& mesh)
{
	CopyMesh(gMeshHead, mesh);
}

// Render 1x1 quad for reference
//DebugShaderType type = gDebugShader;
//gDebugShader = WIRE;
//SetDepthTest(false);
//SetWireframes(true);
//DrawPlaneXY({}, 1.0f, 1.0f, V3_ONE);
//SetWireframes(false);
//SetDepthTest(true);
//gDebugShader = type;
