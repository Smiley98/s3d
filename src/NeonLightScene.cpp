#include "NeonLightScene.h"
#include "ImageLoader.h"
#include "HexagonGrid.h"
#include "Render.h"
#include "Time.h"
#include <cassert>
#include <imgui/imgui.h>

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

static Texture2D fTexBuilding;		// 1x1 white
static Texture2D fTexHexagonGrid;	// Generated via shader

static Vector3 fDirectionLightDirection = V3_FORWARD * -1.0f;
static Vector3 fDirectionLightColor = V3_ONE;

static Vector3 fPointLightPosition = V3_FORWARD * 15.0f + V3_UP * 10.0f;
static Vector3 fPointLightColor = V3_RIGHT;
static float fPointLightRadius = 5.0f;

static bool fDrawLightWireframes = false;
static bool fDrawRenderTargets = false;
static bool fDrawDirectionLight = false;

static bool fDrawBuildings = true;
static bool fDrawGround = true;

// Test world-space vs view-space conversion (g-buffer positions & normals change, lighting stays the same)
static bool fViewSpace = true;
static float fHexagonCount = 10.0f;

struct PointLight
{
	Vector3 position = V3_ZERO;
	Vector3 color = V3_ONE;
	float radius = 1.0f;
};

static PointLight fLights[LIGHT_COUNT];

void GenHexagonGrid();

void DrawGeometry();
void DrawDirectionLight();
void DrawLightVolumes();
void DrawRenderTargets();

void DrawObject(const Mesh& mesh, Matrix world, Texture2D texture);
void DrawLight(const PointLight& light);
void DrawLightWireframes(const PointLight& light);

void NeonLightScene::OnLoad()
{
	gCamera = FromView(LookAt({ 0.0f, -45.0f, 65.0f }, V3_ZERO, V3_UP));

	// Building texture
	{
		uint32_t pixel = 0xFFFFFFFF;
		CreateTexture2D(&fTexBuilding, 1, 1, GL_RGB8, GL_RGBA, GL_UNSIGNED_BYTE, GL_NEAREST, &pixel);
	}

	// Ground texture
	CreateTexture2D(&fTexHexagonGrid, SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, GL_NEAREST);
	GenHexagonGrid();

	// Render target channels are not padded to be RGBA since float-channels are most likely NOT padded by GPU.
	// Additionally, working with the exact channel count removes ambiguity of how the textures are being used.
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

void NeonLightScene::OnUnload()
{
	DestroyFramebuffer(&fGeometryBuffer);
	DestroyTexture2D(&fDepthRT);
	for (int i = 0; i < RenderTargets::COUNT; i++)
		DestroyTexture2D(&fColorsRT[i]);

	DestroyTexture2D(&fTexHexagonGrid);
	DestroyTexture2D(&fTexBuilding);
}

void NeonLightScene::OnUpdate(float dt)
{
	UpdateFpsCameraDefault(gCamera, dt);
	gView = ToView(gCamera);
	gProj = Perspective(PI * 0.5f, SCREEN_ASPECT, 0.1f, 1000.0f);
}

void NeonLightScene::OnDraw()
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

void NeonLightScene::OnDrawImGui()
{
	static int space = 0;
	ImGui::RadioButton("World-Space", &space, 0); ImGui::SameLine();
	ImGui::RadioButton("View-Space", &space, 1);
	fViewSpace = space == 1;

	ImGui::Checkbox("Draw G-Buffer", &fDrawRenderTargets);
	ImGui::Checkbox("Draw Light Wireframes", &fDrawLightWireframes);
	ImGui::Checkbox("Direction Light", &fDrawDirectionLight);

	ImGui::Checkbox("Draw Ground", &fDrawGround);
	ImGui::Checkbox("Draw Buildings", &fDrawBuildings);

	if (fDrawDirectionLight)
	{
		ImGui::SliderFloat3("Light Direction", &fDirectionLightDirection.x, -1.0f, 1.0f);
		ImGui::ColorPicker3("Direction Light Color", &fDirectionLightColor.x);
	}

	if (fDrawGround && ImGui::SliderFloat("Grid Resolution", &fHexagonCount, 1.0f, 25.0f))
		GenHexagonGrid();
}

void DrawGeometry()
{
	BindFramebuffer(fGeometryBuffer);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	BindShader(&gShaderDeferredGeometryBuffer);
	SetPipelineState(gPipelineDefault);

	// Ground begin (y min/max = +-50, y min/max = +-90)
	if (fDrawGround)
		DrawObject(gMeshPlane, Scale(100.0f * SCREEN_ASPECT, 100.0f, 1.0f), fTexHexagonGrid);

	// TD building is 10x10x50
	if (fDrawBuildings)
	{
		for (int y = -40; y <= 40; y += 20)
		{
			for (int x = -70; x <= 70; x += 20)
			{
				Matrix world = Translate(x, y, 0.0f);
				DrawObject(gMeshTd, world, fTexBuilding);
			}
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

	Vector3 lightDirection = fDirectionLightDirection;
	lightDirection = fViewSpace ? NormalMatrix(gView) * lightDirection : lightDirection;
	SendVec3("u_lightDirection", lightDirection);
	SendVec3("u_lightColor", fDirectionLightColor);
	SendFloat("u_ambient", 0.05f);
	SendFloat("u_diffuse", 0.25f);

	//SendInt("u_positions", 0); <-- Not used by direction light
	SendInt("u_normals", 1);
	SendInt("u_albedo", 2);
	DrawFsq();

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
	world = fViewSpace ? world * gView : world;
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
	world = fViewSpace ? world * gView : world;

	SendMat4("u_mvp", mvp);
	SendInt("u_positions", 0);
	SendInt("u_normals", 1);
	SendInt("u_albedo", 2);

	Vector4 lightPosition = light.position;
	lightPosition = fViewSpace ? gView * lightPosition : lightPosition;
	SendVec3("u_lightPosition", lightPosition);
	SendVec3("u_lightColor", light.color);
	SendFloat("u_lightRadius", light.radius);

	SendFloat("u_ambient", 0.25f);
	SendFloat("u_diffuse", 1.0f);
	SendFloat("u_specular", 1.0f);

	SendVec3("u_cameraPosition", gCamera.position);
	SendFloat("u_isViewSpace", fViewSpace ? 1.0f : 0.0f);
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
	// Don't convert these to view-space because we want to visualize our lights in world-space xD
}

void GenHexagonGrid()
{
	// FBO only needed when rendering grid so might as well delete it after draw call
	Framebuffer fb;
	CreateFramebuffer(&fb, SCREEN_WIDTH, SCREEN_HEIGHT);
	fb.colors[0] = &fTexHexagonGrid;
	CompleteFramebuffer(&fb);

	Vector3 fg{ 0.8f, 0.85f, 1.0f };
	Vector3 bg = V3_ONE;

	BindFramebuffer(fb);
	BindShader(&gShaderHexagonGridDistance);
	SendVec2("u_resolution", { SCREEN_WIDTH, SCREEN_HEIGHT });
	SendVec3("u_fg_col", fg);
	SendVec3("u_bg_col", bg);
	SendFloat("u_hex_res", fHexagonCount);
	SendFloat("u_hex_thickness", 0.1f);
	DrawFsq();
	UnbindShader();
	UnbindFramebuffer(fb);

	DestroyFramebuffer(&fb);
}

// TODO 1 -- Render light volumes with instancing.
// 16k kb / 64 bytes per matrix = 256 --> maximum amount of matrices we can fit in a UBO (assuming it contains ONLY these matrices).
// Not even worth sending via uniforms since its bandwidth-bound... Just full-send via instancing!

// TODO 2 -- Add shader reflection / UBOs because sending uniforms 1 by 1 is silly (makes it hard to read).
// However, mat3 vs mat4 might cause problems because raymath only has mat4.
// Have to look up std140 and std 430 rules. Not sure how annoying it would be to convert between float9 vs mat4 in struct UniformData.

// TODO 3 -- Add manual light control and/or animated lights?
//ImGui::SliderFloat3("Light Position", &fPointLightPosition.x, -50.0f, 50.0f);
//ImGui::SliderFloat("Light Radius", &fPointLightRadius, 0.0f, 25.0f);
//ImGui::ColorPicker3("Point Light Color", &fPointLightColor.x);
