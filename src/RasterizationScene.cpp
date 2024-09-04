#include "RasterizationScene.h"
#include "Time.h"
#include <imgui/imgui.h>

enum ShaderType : int
{
	FLAT,
	WIRE,
	POSITIONS_WORLD,
	POSITIONS_SCREEN,
	NORMALS_OBJECT,
	NORMALS_WORLD
} fShader{};

bool fTranslate = false;
bool fRotate = false;
bool fScale = false;

Vector3 fColor = V3_ONE;

void RasterizationScene::OnDraw()
{
	float tt = TotalTime();

	Matrix translation = fTranslate ? Translate(cosf(tt), 0.0f, 0.0f) : MatrixIdentity();
	Matrix rotation = fRotate ? RotateY(TotalTime() * 100.0f * DEG2RAD) : MatrixIdentity();
	Matrix scale = fScale ? Scale(cosf(tt), sinf(tt), 1.0f) : MatrixIdentity();

	Matrix world = scale * rotation * translation;
	Matrix view = LookAt({ 0.0f, 0.0f, 5.0f }, V3_ZERO, V3_UP);
	Matrix proj = mProj;
	Matrix mvp = world * view * proj;

	gView = view;
	gProj = proj;
	
	float angle = tt * 100.0f * DEG2RAD;
	DrawRectangle({ -2.0f, 1.0f }, 1.0f, 2.0f, { 1.0f, 0.0f, 0.0f }, angle);
	DrawRectangle({  2.0f, 1.0f }, 2.0f, 1.0f, { 1.0f, 0.5f, 0.0f }, angle);
	DrawCapsuleH({ -2.0f, -1.0f }, 0.5f, 1.0f, { 0.5f, 0.0f, 1.0f }, angle);
	DrawCapsuleV({  2.0f, -1.0f }, 0.5f, 1.0f, { 0.0f, 1.0f, 1.0f }, angle);

	switch (fShader)
	{
	case FLAT:
		DrawMeshFlat(*mMesh, mvp, fColor);
		break;

	case WIRE:
		DrawMeshWireframes(*mMesh, mvp, fColor);
		break;

	case POSITIONS_WORLD:
		DrawMeshPositionsWorld(*mMesh, mvp, world);
		break;

	case POSITIONS_SCREEN:
		DrawMeshPositionsScreen(*mMesh, mvp);
		break;

	case NORMALS_OBJECT:
		DrawMeshNormals(*mMesh, mvp, MatrixIdentity());
		break;

	case NORMALS_WORLD:
		DrawMeshNormals(*mMesh, mvp, world);
		break;

	default:
		assert(false, "Invalid Shader");
		break;
	}
}

void RasterizationScene::OnDrawImGui()
{
	static const char* meshNames[] =
	{
		"Triangle",
		"Plane",
		"Cube",
		"Circle",
		"Semicircle",
		"Sphere",
		"Head"
	};

	static const char* shaderNames[] =
	{
		"Flat",
		"Wireframe",
		"World-Space Positions",
		"Screen-Space Positions",
		"Object-Space Normals",
		"World-Space Normals"
	};

	static Mesh* meshes[] =
	{
		&gMeshTriangle,
		&gMeshPlane,
		&gMeshCube,
		&gMeshCircle,
		&gMeshSemicircle,
		&gMeshSphere,
		&gMeshHead
	};

	static Matrix projections[] =
	{
		Ortho(-1.0f, 1.0f, -1.0f, 1.0f, -10.0f, 10.0f),
		Perspective(75.0f * DEG2RAD, 1.0f, 0.001f, 100.0f)
	};

	static int projIndex = 1;
	ImGui::RadioButton("Orthographic", &projIndex, 0); ImGui::SameLine();
	ImGui::RadioButton("Perspective", &projIndex, 1);
	mProj = projections[projIndex];

	ImGui::Checkbox("Translate", &fTranslate); ImGui::SameLine();
	ImGui::Checkbox("Rotate", &fRotate); ImGui::SameLine();
	ImGui::Checkbox("Scale", &fScale);
	
	static int meshIndex = 0;
	ImGui::Combo("Meshes", &meshIndex, meshNames, IM_ARRAYSIZE(meshNames));
	mMesh = meshes[meshIndex];

	ImGui::Combo("Shaders", (int*)&fShader, shaderNames, IM_ARRAYSIZE(shaderNames));

	ImGui::ColorPicker3("Colour", &fColor.x);
	//ImGui::ShowDemoWindow();
}
