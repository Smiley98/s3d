#include "DeferredScene.h"
#include "Time.h"
#include "Render.h"
#include "ImageLoader.h"

static Texture2D fTextureAlbedo;
static Framebuffer fGeometryBuffer;

static Vector3 fLightPosition = V3_ZERO;
static Vector3 fLightColor = V3_ONE;

void DeferredScene::OnLoad()
{
	gCamera = FromView(LookAt({ 0.0f, 0.0f, 20.0f }, V3_ZERO, V3_UP));

	// Convert from 4-channels in storage to 3-channels in memory (format = SSD, internal format = VRAM)
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

void DeferredScene::OnUnload()
{
	DestroyFramebuffer(&fGeometryBuffer);
	DestroyTexture2D(&fTextureAlbedo);
}

void DeferredScene::OnUpdate(float dt)
{
	float tt = TotalTime();
	UpdateFpsCameraDefault(gCamera, dt);
	gView = ToView(gCamera);
	gProj = Perspective(PI * 0.5f, SCREEN_ASPECT, 0.1f, 1000.0f);
	fLightPosition = { cosf(tt) * 5.0f, 0.0f, 10.0f };
}

void DeferredScene::OnDraw()
{
	Mesh mesh;
	Matrix world = Scale(V3_ONE * 10.0f);
	Matrix mvp = world * gView * gProj;

	BindFramebuffer(fGeometryBuffer);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	BindTexture2D(fTextureAlbedo, 0);
	BindShader(&gShaderDeferred);

	SendMat4("u_mvp", mvp);
	SendMat4("u_world", world);
	SendMat3("u_normal", NormalMatrix(world));
	SendInt("u_tex", 0);
	DrawMesh(gMeshHead);

	UnbindShader();
	UnbindTexture2D(fTextureAlbedo, 0);
	UnbindFramebuffer(fGeometryBuffer);

	float hw = SCREEN_WIDTH * 0.5f;
	float hh = SCREEN_HEIGHT * 0.5f;

	// Positions (0, bottom-left)
	//glViewport(0, 0, hw, hh);
	//DrawColor(fGeometryBuffer, 0);
	//
	//// Normals (1, bottom-right)
	//glViewport(hw, 0, hw, hh);
	//DrawColor(fGeometryBuffer, 1);
	//
	//// Albedo (2, top-left)
	//glViewport(0, hh, hw, hh);
	//DrawColor(fGeometryBuffer, 2);
	//
	//// Final render (0, 1, 2, top-right)
	//glViewport(hw, hh, hw, hh);

	BindTexture2D(fGeometryBuffer.colors[0], 0);
	BindTexture2D(fGeometryBuffer.colors[1], 1);
	BindTexture2D(fGeometryBuffer.colors[2], 2);
	BindShader(&gShaderDeferredLighting);

	world = Scale(V3_ONE * 5.0f) * Translate(fLightPosition);
	mvp = world * gView * gProj;
	SendMat4("u_mvp", mvp);
	SendInt("u_positions", 0);
	SendInt("u_normals", 1);
	SendInt("u_albedo", 2);
	SendVec2("u_resolution", { SCREEN_WIDTH, SCREEN_HEIGHT });
	SendVec3("u_lightPosition", fLightPosition);
	SendVec3("u_lightColor", fLightColor);

	GLenum faceCulling = FaceCulling();
	SetFaceCulling(GL_FRONT);
	DrawMesh(gMeshSphere);
	SetFaceCulling(faceCulling);

	UnbindShader();
	UnbindTexture2D(fGeometryBuffer.colors[2], 2);
	UnbindTexture2D(fGeometryBuffer.colors[1], 1);
	UnbindTexture2D(fGeometryBuffer.colors[0], 0);

	DrawMeshWireframes(gMeshSphere, world, fLightColor);
	// No need to test blitting cause I can just disable the depth-test when rendering light volumes...
}
