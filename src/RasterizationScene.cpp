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
Vector3 fPosition = V3_ZERO;

void RasterizationScene::OnDraw()
{
	float tt = TotalTime();

	Matrix translation = Translate(fPosition) *
		(fTranslate ? Translate(cosf(tt), 0.0f, 0.0f) : MatrixIdentity());
	
	Matrix rotation =
		fRotate ? RotateY(tt * 100.0f * DEG2RAD) : MatrixIdentity();
	
	Matrix scale =
		fScale ? Scale(cosf(tt), sinf(tt), 1.0f) : MatrixIdentity();

	Matrix world = scale * rotation * translation;
	Matrix mvp = world * mView * mProj;

	// Temporary hack to test primitive-rendering
	SetView(mView);
	SetProj(mProj);

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

void GenHead(Mesh* mesh)
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

	static const char* shaderNames[] =
	{
		"Flat",
		"Wireframe",
		"World-Space Positions",
		"Screen-Space Positions",
		"Object-Space Normals",
		"World-Space Normals"
	};

	using MeshGenerator = void(*)(Mesh*);
	MeshGenerator generators[] =
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

	ImGui::SliderFloat3("Object Position", &fPosition.x, -10.0f, 10.0f);

	static Vector3 camPos{ 0.0f, 0.0f, 5.0f };
	ImGui::SliderFloat3("Camera Position", &camPos.x, -10.0f, 10.0f);
	mView = LookAt(camPos, V3_ZERO, V3_UP);

	static int meshIndex = 0;
	int meshCount = IM_ARRAYSIZE(meshNames);
	bool gen = ImGui::Combo("Meshes", &meshIndex, meshNames, meshCount);
	if (gen)
	{
		DestroyMesh(mMesh);
		generators[meshIndex](mMesh);
	}

	ImGui::Combo("Shaders", (int*)&fShader, shaderNames, IM_ARRAYSIZE(shaderNames));
	ImGui::ColorPicker3("Colour", &fColor.x);
	//ImGui::ShowDemoWindow();
}
