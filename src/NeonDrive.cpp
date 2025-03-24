#include "NeonDrive.h"
#include "ImageLoader.h"
#include "HexagonGrid.h"
#include "Render.h"
#include "Time.h"
#include <cassert>
#include <imgui/imgui.h>

// TODO -- Go crazy and instance 512 lights & see if it runs.
// 16k kb / 64 bytes per matrix = 256 --> maximum amount of matrices we can fit in a UBO (assuming it contains ONLY these matrices).
// Not even worth sending via uniforms since its bandwidth-bound... Just full-send via instancing!
constexpr int LIGHT_COUNT = 64;

enum RenderTargets
{
	POSITIONS,
	NORMALS,
	ALBEDO,
	ACCUMULATION,
	COUNT
};

static Framebuffer fGeometryBuffer;
static Texture2D fColorsRT[RenderTargets::COUNT];
static Texture2D fDepthRT;

// Just a 1x1 white texture
static Texture2D fTexBuilding;

static Vector3 fDirectionLightDirection = V3_FORWARD * -1.0f;
static Vector3 fDirectionLightColor = V3_ONE;

static Vector3 fPointLightPosition = V3_FORWARD * 15.0f + V3_UP * 10.0f;
static Vector3 fPointLightColor = V3_RIGHT;
static float fPointLightRadius = 5.0f;

static bool fDrawLightWireframes = false;
static bool fDrawRenderTargets = false;
static bool fDrawDirectionLight = false;

struct PointLight
{
	Vector3 position = V3_ZERO;
	Vector3 color = V3_ONE;
	float radius = 1.0f;
};

static PointLight fLights[LIGHT_COUNT];

void DrawGeometry();
void DrawDirectionLight();
void DrawLightVolumes();
void DrawRenderTargets();

void DrawObject(const Mesh& mesh, Matrix world, Texture2D texture);
void DrawLight(const PointLight& light);
void DrawLightWireframes(const PointLight& light);

void NeonDriveScene::OnLoad()
{
	gCamera = FromView(LookAt({ 0.0f, -25.0f, 20.0f }, V3_ZERO, V3_UP));

	// World's most elaborate texture xD xD xD
	{
		uint32_t pixel = 0xFFFFFFFF;
		CreateTexture2D(&fTexBuilding, 1, 1, GL_RGB8, GL_RGBA, GL_UNSIGNED_BYTE, GL_NEAREST, &pixel);
	}

	CreateTexture2D(&fColorsRT[RenderTargets::POSITIONS], SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGB16F, GL_RGB, GL_FLOAT, GL_NEAREST);
	CreateTexture2D(&fColorsRT[RenderTargets::NORMALS], SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGB16F, GL_RGB, GL_FLOAT, GL_NEAREST);
	CreateTexture2D(&fColorsRT[RenderTargets::ALBEDO], SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE, GL_NEAREST);
	CreateTexture2D(&fColorsRT[RenderTargets::ACCUMULATION], SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE, GL_NEAREST);
	CreateTexture2D(&fDepthRT, SCREEN_WIDTH, SCREEN_HEIGHT, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT, GL_NEAREST);

	CreateFramebuffer(&fGeometryBuffer, SCREEN_WIDTH, SCREEN_HEIGHT);
	for (int i = 0; i < RenderTargets::COUNT; i++)
		fGeometryBuffer.colors[i] = &fColorsRT[i];
	fGeometryBuffer.depth = &fDepthRT;
	CompleteFramebuffer(&fGeometryBuffer);

	for (int i = 0; i < LIGHT_COUNT; i++)
	{
		PointLight& l = fLights[i];
		l.position.x = Random(-50.0f * SCREEN_ASPECT, 50.0f * SCREEN_ASPECT);
		l.position.y = Random(-50.0f, 50.0f);
		l.position.z = Random(0.0f, 50.0f);
		l.color.x = Random(0.0f, 1.0f);
		l.color.y = Random(0.0f, 1.0f);
		l.color.z = Random(0.0f, 1.0f);
		l.radius = Random(5.0f, 25.0f);
	}
}

void NeonDriveScene::OnUnload()
{
	for (int i = 0; i < RenderTargets::COUNT; i++)
		DestroyTexture2D(&fColorsRT[i]);
	DestroyTexture2D(&fDepthRT);

	DestroyTexture2D(&fTexBuilding);
	DestroyFramebuffer(&fGeometryBuffer);
}

void NeonDriveScene::OnUpdate(float dt)
{
	UpdateFpsCameraDefault(gCamera, dt);
	gView = ToView(gCamera);
	gProj = Perspective(PI * 0.5f, SCREEN_ASPECT, 0.1f, 1000.0f);
}

void NeonDriveScene::OnDraw()
{
	// 1. Render geometry (g-buffer write)
	DrawGeometry();

	// 2. Render lights (light-buffer write)
	glEnable(GL_BLEND); 
	glBlendFunc(GL_ONE, GL_ONE);
	glBlendEquation(GL_FUNC_ADD);
	if (fDrawDirectionLight)
		DrawDirectionLight();
	DrawLightVolumes();
	glDisable(GL_BLEND);

	// 3. Visualize either the entire G-buffer or the final render
	if (fDrawRenderTargets)
		DrawRenderTargets();
	else
		DrawFsqTexture(fColorsRT[RenderTargets::ACCUMULATION], 0);
	
	// 4. Optional light volumes visualization
	if (fDrawLightWireframes)
	{
		if (fDrawRenderTargets)
		{
			float hw = SCREEN_WIDTH * 0.5f;
			float hh = SCREEN_HEIGHT * 0.5f;
			glViewport(hw, hh, hw, hh);
		}

		for (int i = 0; i < LIGHT_COUNT; i++)
			DrawLightWireframes(fLights[i]);

		if (fDrawRenderTargets)
			glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	}
}

void NeonDriveScene::OnDrawImGui()
{
	ImGui::Checkbox("Draw Light Wireframes", &fDrawLightWireframes);
	ImGui::Checkbox("Draw G-Buffer", &fDrawRenderTargets);
	ImGui::Checkbox("Direction Light", &fDrawDirectionLight);
	if (fDrawDirectionLight)
	{
		ImGui::SliderFloat3("Light Direction", &fDirectionLightDirection.x, -1.0f, 1.0f);
		ImGui::ColorPicker3("Direction Light Color", &fDirectionLightColor.x);
	}
	
	// TODO -- Add manual light control?
	//ImGui::SliderFloat3("Light Position", &fPointLightPosition.x, -50.0f, 50.0f);
	//ImGui::SliderFloat("Light Radius", &fPointLightRadius, 0.0f, 25.0f);
	//ImGui::ColorPicker3("Point Light Color", &fPointLightColor.x);
}

void DrawGeometry()
{
	BindFramebuffer(fGeometryBuffer);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	BindShader(&gShaderDeferredGeometryBuffer);
	SetPipelineState(gPipelineDefault);

	// Ground begin (y min/max = +-50, y min/max = +-90)
	DrawObject(gMeshPlane, Scale(100.0f * SCREEN_ASPECT, 100.0f, 1.0f), GetHexagonGrid());

	// TD building is 10x10x50
	for (int y = -40; y <= 40; y += 20)
	{
		for (int x = -70; x <= 70; x += 20)
		{
			Matrix world = Translate(x, y, 0.0f);
			DrawObject(gMeshTd, world, fTexBuilding);
		}
	}

	UnbindShader();
	UnbindFramebuffer(fGeometryBuffer);
}

void DrawDirectionLight()
{
	DrawBuffers drawBuffers{};
	drawBuffers[3] = true;//{ GL_NONE, GL_NONE, GL_NONE, GL_COLOR_ATTACHMENT3 }
	BindFramebuffer(fGeometryBuffer, &drawBuffers);

	BindTexture2D(fColorsRT[RenderTargets::POSITIONS], 0);
	BindTexture2D(fColorsRT[RenderTargets::NORMALS], 1);
	BindTexture2D(fColorsRT[RenderTargets::ALBEDO], 2);
	BindShader(&gShaderDeferredDirectionLight);

	SendVec3("u_lightDirection", Normalize(fDirectionLightDirection));
	SendVec3("u_lightColor", fDirectionLightColor);
	SendFloat("u_ambient", 0.05f);
	SendFloat("u_diffuse", 0.25f);

	SendInt("u_positions", 0);
	SendInt("u_normals", 1);
	SendInt("u_albedo", 2);
	DrawFsq(); // <-- Draws with depth test & depth write disabled by default

	UnbindShader();
	UnbindTexture2D(fColorsRT[RenderTargets::ALBEDO], 2);
	UnbindTexture2D(fColorsRT[RenderTargets::NORMALS], 1);
	UnbindTexture2D(fColorsRT[RenderTargets::POSITIONS], 0);

	UnbindFramebuffer(fGeometryBuffer);
}

void DrawLightVolumes()
{
	DrawBuffers drawBuffers{};
	drawBuffers[3] = true;//{ GL_NONE, GL_NONE, GL_NONE, GL_COLOR_ATTACHMENT3 }
	BindFramebuffer(fGeometryBuffer, &drawBuffers);

	BindTexture2D(fColorsRT[RenderTargets::POSITIONS], 0);
	BindTexture2D(fColorsRT[RenderTargets::NORMALS], 1);
	BindTexture2D(fColorsRT[RenderTargets::ALBEDO], 2);
	BindShader(&gShaderDeferredLightVolumes);

	for (int i = 0; i < LIGHT_COUNT; i++)
		DrawLight(fLights[i]);

	UnbindShader();
	UnbindTexture2D(fColorsRT[RenderTargets::ALBEDO], 2);
	UnbindTexture2D(fColorsRT[RenderTargets::NORMALS], 1);
	UnbindTexture2D(fColorsRT[RenderTargets::POSITIONS], 0);

	UnbindFramebuffer(fGeometryBuffer);
}

void DrawRenderTargets()
{
	float hw = SCREEN_WIDTH * 0.5f;
	float hh = SCREEN_HEIGHT * 0.5f;

	glViewport(0, 0, hw, hh);	//Bottom-left
	DrawFsqTexture(fColorsRT[RenderTargets::POSITIONS], 0);

	glViewport(hw, 0, hw, hh);	// Bottom-right
	DrawFsqTexture(fColorsRT[RenderTargets::NORMALS], 1);

	glViewport(0, hh, hw, hh);	// Top-left
	DrawFsqTexture(fColorsRT[RenderTargets::ALBEDO], 2);

	glViewport(hw, hh, hw, hh);	// Top-right
	DrawFsqTexture(fColorsRT[RenderTargets::ACCUMULATION], 3);

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void DrawObject(const Mesh& mesh, Matrix world, Texture2D texture)
{
	Matrix mvp = world * gView * gProj;
	SendMat4("u_mvp", mvp);
	SendMat4("u_world", world);
	SendMat3("u_normal", NormalMatrix(world));
	SendInt("u_tex", 0);
	BindTexture2D(texture, 0);
	DrawMesh(mesh);
	UnbindTexture2D(texture, 0);
}

void DrawLight(const PointLight& light)
{
	Matrix world = Scale(V3_ONE * light.radius) * Translate(light.position);
	Matrix mvp = world * gView * gProj;

	SendMat4("u_mvp", mvp);
	SendInt("u_positions", 0);
	SendInt("u_normals", 1);
	SendInt("u_albedo", 2);

	SendVec3("u_lightPosition", light.position);
	SendVec3("u_lightColor", light.color);
	SendFloat("u_lightRadius", light.radius);

	SendFloat("u_ambient", 0.25f);
	SendFloat("u_diffuse", 1.0f);
	SendFloat("u_specular", 1.0f);

	SendVec3("u_cameraPosition", gCamera.position);
	SendVec2("u_screen", { SCREEN_WIDTH, SCREEN_HEIGHT });

	PipelineState ps = gPipelineNoDepth;
	ps.cullFace = GL_FRONT;
	SetPipelineState(ps);
	DrawMesh(gMeshSphere);
}

void DrawLightWireframes(const PointLight& light)
{
	Matrix world = Scale(V3_ONE * light.radius) * Translate(light.position);
	DrawMeshWireframes(gMeshSphere, world, light.color);
}

// Add shader reflection / UBOs because sending uniforms 1 by 1 is silly (makes it hard to read).
// However, mat3 vs mat4 might cause problems because raymath only has mat4.
// Have to look up std140 and std 430 rules. Not sure how annoying it would be to convert between float9 vs mat4 in struct UniformData.
