#include "NeonDrive.h"
#include "ImageLoader.h"
#include "HexagonGrid.h"
#include "Render.h"
#include "Time.h"
#include <cassert>
#include <imgui/imgui.h>

static Texture2D fTextureWhite;
static Texture2D fTextureAlbedo;
static Framebuffer fGeometryBuffer;

static Vector3 fDirectionLightDirection = V3_RIGHT * -1.0f + V3_UP + V3_FORWARD * -1.0f;
static Vector3 fDirectionLightColor = V3_ONE;

static Vector3 fPointLightPosition = V3_FORWARD * 35.0f - V3_UP * 10.0f;
static Vector3 fPointLightColor = V3_RIGHT;
static float fPointLightRadius = 10.0f;

void DrawGeometry();
void DrawDirectionLight();
void DrawLightVolumes();

void NeonDriveScene::OnLoad()
{
	gCamera = FromView(LookAt({ 0.0f, -20.0f, 65.0f }, V3_ZERO, V3_UP));

	// World's most elaborate texture xD xD xD
	{
		uint32_t pixel = 0xFFFFFFFF;
		CreateTexture2D(&fTextureWhite, 1, 1, GL_RGB8, GL_RGBA, GL_UNSIGNED_BYTE, GL_NEAREST, &pixel);
	}

	// Original format = RGBA
	{
		int w, h, c;
		uint8_t* pixels = LoadImage2D("./assets/textures/african_head_diffuse.png", &w, &h, &c);
		CreateTexture2D(&fTextureAlbedo, w, h, GL_RGB8, GL_RGBA, GL_UNSIGNED_BYTE, GL_LINEAR, pixels);
		UnloadImage(pixels);
	}

	// Positions, normals, and albedo are all 3-component. Can convert to RGBA for albedo-specular.
	CreateFramebuffer(&fGeometryBuffer, SCREEN_WIDTH, SCREEN_HEIGHT);
	AddColor(&fGeometryBuffer, GL_RGB16F, GL_RGB, GL_FLOAT, GL_NEAREST);		// Positions
	AddColor(&fGeometryBuffer, GL_RGB16F, GL_RGB, GL_FLOAT, GL_NEAREST);		// Normals
	AddColor(&fGeometryBuffer, GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE, GL_NEAREST);	// Albedo
	AddDepth(&fGeometryBuffer);
	CompleteFramebuffer(&fGeometryBuffer);
}

void NeonDriveScene::OnUnload()
{
	DestroyFramebuffer(&fGeometryBuffer);
	DestroyTexture2D(&fTextureAlbedo);
	DestroyTexture2D(&fTextureWhite);
}

void NeonDriveScene::OnUpdate(float dt)
{
	UpdateFpsCameraDefault(gCamera, dt);
	gView = ToView(gCamera);
	gProj = Perspective(PI * 0.5f, SCREEN_ASPECT, 0.1f, 1000.0f);
}

void NeonDriveScene::OnDraw()
{
	DrawGeometry();
	//DrawDepth(fGeometryBuffer);

	DrawDirectionLight();
	DrawLightVolumes();
}

void NeonDriveScene::OnDrawImGui()
{
	ImGui::SliderFloat3("Light Position", &fPointLightPosition.x, -50.0f, 50.0f);
	ImGui::SliderFloat("Light Radius", &fPointLightRadius, 0.0f, 25.0f);

	ImGui::Separator();
	ImGui::SliderFloat3("Light Direction", &fDirectionLightDirection.x, -1.0f, 1.0f);
	
	//ImGui::ColorPicker3("Point Light Color", &fPointLightColor.x);
	//ImGui::ColorPicker3("Direction Light Color", &fDirectionLightColor.x);
}

void DrawGeometry()
{
	Matrix world = MatrixIdentity();
	Matrix mvp = MatrixIdentity();

	BindFramebuffer(fGeometryBuffer);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	BindShader(&gShaderDeferredGeometryBuffer);
	SetPipelineState(gPipelineDefault);

	// Ground begin
	world = Scale(100.0f * SCREEN_ASPECT, 100.0f, 1.0f);
	mvp = world * gView * gProj;
	SendMat4("u_mvp", mvp);
	SendMat4("u_world", world);
	SendMat3("u_normal", NormalMatrix(world));
	SendInt("u_tex", 0);
	BindTexture2D(GetHexagonGrid(), 0);
	DrawMesh(gMeshPlane);
	UnbindTexture2D(GetHexagonGrid(), 0);
	// Ground end

	// TD begin
	world = MatrixIdentity();
	mvp = world * gView * gProj;
	SendMat4("u_mvp", mvp);
	SendMat4("u_world", world);
	SendMat3("u_normal", NormalMatrix(world));
	SendInt("u_tex", 0);
	BindTexture2D(fTextureWhite, 0);
	DrawMesh(gMeshTd);
	UnbindTexture2D(fTextureWhite, 0);
	// TD end

	UnbindShader();
	UnbindFramebuffer(fGeometryBuffer);
}

void DrawDirectionLight()
{
	BindTexture2D(fGeometryBuffer.colors[0], 0);
	BindTexture2D(fGeometryBuffer.colors[1], 1);
	BindTexture2D(fGeometryBuffer.colors[2], 2);

	BindShader(&gShaderDeferredDirectionLight);
	SendVec3("u_lightDirection", Normalize(fDirectionLightDirection));
	SendVec3("u_lightColor", fDirectionLightColor);
	SendFloat("u_ambient", 0.05f);
	SendFloat("u_diffuse", 0.25f);

	SendInt("u_positions", 0);
	SendInt("u_normals", 1);
	SendInt("u_albedo", 2);
	DrawFsq();
	UnbindShader();

	UnbindTexture2D(fGeometryBuffer.colors[2], 2);
	UnbindTexture2D(fGeometryBuffer.colors[1], 1);
	UnbindTexture2D(fGeometryBuffer.colors[0], 0);
}

void DrawLightVolumes()
{
	BindTexture2D(fGeometryBuffer.colors[0], 0);
	BindTexture2D(fGeometryBuffer.colors[1], 1);
	BindTexture2D(fGeometryBuffer.colors[2], 2);
	BindShader(&gShaderDeferredLightVolumes);

	Matrix S = Scale(V3_ONE * fPointLightRadius);
	Matrix R = MatrixIdentity();
	Matrix T = Translate(fPointLightPosition);
	Matrix world = S * R * T;
	Matrix mvp = world * gView * gProj;

	SendMat4("u_mvp", mvp);
	SendInt("u_positions", 0);
	SendInt("u_normals", 1);
	SendInt("u_albedo", 2);

	SendVec3("u_lightPosition", fPointLightPosition);
	SendVec3("u_lightColor", fPointLightColor);
	SendFloat("u_ambient", 0.25f);
	SendFloat("u_diffuse", 1.0f);

	SendVec2("u_viewportSize", { SCREEN_WIDTH, SCREEN_HEIGHT });
	SendVec2("u_viewportOffset", V2_ZERO);

	//PipelineState ps = gPipelineDefault;
	//ps.depthTest = true;
	//ps.depthWrite = false;
	//ps.cullFace = GL_FRONT;
	//ps.depthFunc = GL_GEQUAL;

	// Light is applied even when behind the TD building. See if GEQUAL can fix this!
	PipelineState ps = gPipelineNoDepth;
	ps.depthTest = true;
	ps.cullFace = GL_FRONT;
	//ps.depthFunc = GL_GEQUAL;

	SetPipelineState(ps);
	DrawMesh(gMeshSphere);
	SetPipelineState(gPipelineDefault);

	UnbindShader();
	UnbindTexture2D(fGeometryBuffer.colors[2], 2);
	UnbindTexture2D(fGeometryBuffer.colors[1], 1);
	UnbindTexture2D(fGeometryBuffer.colors[0], 0);
}
