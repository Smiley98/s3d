#include "RasterizationScene.h"
#include "Time.h"
#include "Window.h"
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

static Matrix fView = MatrixIdentity();
static Matrix fProj = MatrixIdentity();

Mesh fMesh;
Vector3 fColor = V3_ONE;
Vector3 fPosition = V3_ZERO;

Vector3 fCamPos{ 0.0f, 0.0f, 5.0f };
Quaternion fCamRot = QuaternionIdentity();

// Improvement idea:
// Calc pitch & yaw every frame
// Append to quaternion
// **cam pos & cam rot persist in camera-space**

// View = inv(Camera)
// Camera is same as world -- rotation (pitch-yaw) then position
Matrix LookAtFps(Vector3 eye, Quaternion rot)
{
	//Matrix rotation = ToMatrix(rot);
	//Matrix translation = Translate(eye);
	//Matrix camera = rotation * translation;
	//Matrix view = Invert(camera);
	return Invert(ToMatrix(rot) * Translate(eye));
}

void SwitchShader(Matrix mvp, Matrix world);

void RasterizationScene::OnCreate()
{
	GenEquilateral(fMesh);
}

void RasterizationScene::OnDestroy()
{
	DestroyMesh(fMesh);
}

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
	Matrix mvp = world * fView * fProj;

	//Vector2 delta = MouseDelta();
	//float pitch = delta.x;
	//float yaw = delta.y;
	//fCamDir = fCamDir * FromEuler(pitch, yaw, 0.0f);

	//float speed = Length(delta);

	//printf("%f %f\n", delta.x, delta.y);

	// Temporary hack to test primitive-rendering
	SetView(fView);
	SetProj(fProj);

	// Capsule is oriented along +Z so you must rotate accordingly!
	DrawSpherocylinder({}, 1.0f, 2.0f, { 1.0f, 0.0f, 0.0f },
		ToMatrix(FromEuler(-PI * 0.25f, PI * 0.5f, 0.0f)));

	SwitchShader(mvp, world);
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

	static const char* shaderNames[] =
	{
		"Flat",
		"Wireframe",
		"World-Space Positions",
		"Screen-Space Positions",
		"Object-Space Normals",
		"World-Space Normals"
	};

	static Matrix projections[] =
	{
		Ortho(-1.0f, 1.0f, -1.0f, 1.0f, -10.0f, 10.0f),
		Perspective(75.0f * DEG2RAD, 1.0f, 0.001f, 100.0f)
	};

	static int projIndex = 1;
	ImGui::RadioButton("Orthographic", &projIndex, 0); ImGui::SameLine();
	ImGui::RadioButton("Perspective", &projIndex, 1);
	fProj = projections[projIndex];

	ImGui::Checkbox("Translate", &fTranslate); ImGui::SameLine();
	ImGui::Checkbox("Rotate", &fRotate); ImGui::SameLine();
	ImGui::Checkbox("Scale", &fScale);

	ImGui::SliderFloat3("Object Position", &fPosition.x, -10.0f, 10.0f);

	static Vector3 camPos{ 0.0f, 0.0f, 5.0f };
	static float pitch = 0.0f;
	static float yaw = 0.0f;

	ImGui::SliderFloat3("Camera Position", &camPos.x, -10.0f, 10.0f);
	ImGui::SliderAngle("Pitch", &pitch);
	ImGui::SliderAngle("Yaw", &yaw);
	fView = LookAtFps(camPos, FromEuler(pitch, yaw, 0.0f));
	//fView = LookAt(camPos, V3_ZERO, V3_UP);

	static int meshIndex = 0;
	bool gen = ImGui::Combo("Meshes", &meshIndex, meshNames, IM_ARRAYSIZE(meshNames));
	if (gen)
	{
		DestroyMesh(fMesh);
		generators[meshIndex](fMesh);
	}

	ImGui::Combo("Shaders", (int*)&fShader, shaderNames, IM_ARRAYSIZE(shaderNames));
	ImGui::ColorPicker3("Colour", &fColor.x);
	//ImGui::ShowDemoWindow();
}

void SwitchShader(Matrix mvp, Matrix world)
{
	switch (fShader)
	{
	case FLAT:
		DrawMeshFlat(fMesh, mvp, fColor);
		break;

	case WIRE:
		DrawMeshWireframes(fMesh, mvp, fColor);
		break;

	case POSITIONS_WORLD:
		DrawMeshPositionsWorld(fMesh, mvp, world);
		break;

	case POSITIONS_SCREEN:
		DrawMeshPositionsScreen(fMesh, mvp);
		break;

	case NORMALS_OBJECT:
		DrawMeshNormals(fMesh, mvp, MatrixIdentity());
		break;

	case NORMALS_WORLD:
		DrawMeshNormals(fMesh, mvp, world);
		break;

	default:
		assert(false, "Invalid Shader");
		break;
	}
}
