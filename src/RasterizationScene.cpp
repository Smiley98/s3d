#include "RasterizationScene.h"
#include "Window.h"
#include "Time.h"
#include "Render.h"
#include "Camera.h"
#include <imgui/imgui.h>

bool fTranslate = false;
bool fRotate = false;
bool fScale = false;

Vector3 fColor = V3_ONE;
Vector3 fPosition = V3_ZERO;

Matrix fWorld;
Mesh fMesh;

int fMeshIndex = 0;
int fProjIndex = 1;
Matrix fProjections[] =
{
	Ortho(-10.0f * SCREEN_ASPECT, 10.0f * SCREEN_ASPECT, -10.0f, 10.0f, 0.1f, 10.0f),
	Perspective(75.0f * DEG2RAD, SCREEN_ASPECT, 0.1f, 10.0f)
};

void RasterizationScene::OnLoad()
{
	CreateMesh(&fMesh, (MeshType)fMeshIndex);
	gCamera.position = { 0.0f, 0.0f, 5.0f };
}

void RasterizationScene::OnUnload()
{
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

	BindCubemap(gSkybox);
	BindShader(&gShaderEnvironment);
	Matrix mvp = fWorld * gView * gProj;
	SendMat4("u_mvp", mvp);
	SendMat4("u_world", fWorld);
	SendMat3("u_normal", NormalMatrix(fWorld));
	DrawMesh(gMeshHead);
	UnbindShader();
	UnbindCubemap(gSkybox);
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
		CreateMesh(&fMesh, (MeshType)fMeshIndex);
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
