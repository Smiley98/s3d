#include "DeferredScene.h"
#include "Render.h"
#include "ImageLoader.h"

static Texture2D fTextureAlbedo;
static Framebuffer fGeometryBuffer;

void DeferredScene::OnLoad()
{
	gCamera = FromView(LookAt({ 0.0f, 0.0f, 50.0f }, V3_ZERO, V3_UP));

	// Note - Shaders only use .rgb when sampling diffuse textures at the moment (4th color channel not necessary).
	CreateFramebuffer(&fGeometryBuffer, SCREEN_WIDTH, SCREEN_HEIGHT);
	AddColor(&fGeometryBuffer, GL_RGBA16F, GL_RGBA, GL_FLOAT, GL_NEAREST);			// Positions
	AddColor(&fGeometryBuffer, GL_RGBA16F, GL_RGBA, GL_FLOAT, GL_NEAREST);			// Normals
	AddColor(&fGeometryBuffer, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, GL_NEAREST);	// Albedo-Specular
	AddDepth(&fGeometryBuffer);
	CompleteFramebuffer(&fGeometryBuffer);

	// Convert from 4-channels in storage to 3-channels in memory (format = SSD, internal format = VRAM)
	{
		int w, h, c;
		uint8_t* pixels = LoadImage2D("./assets/textures/african_head_diffuse.png", &w, &h, &c);
		CreateTexture2D(&fTextureAlbedo, w, h, GL_RGB8, GL_RGBA, GL_UNSIGNED_BYTE, GL_LINEAR, pixels);
		UnloadImage(pixels);
	}
}

void DeferredScene::OnUnload()
{
	DestroyTexture2D(&fTextureAlbedo);
	DestroyFramebuffer(&fGeometryBuffer);
}

void DeferredScene::OnUpdate(float dt)
{
	UpdateFpsCameraDefault(gCamera, dt);
	gView = ToView(gCamera);
	gProj = Perspective(PI * 0.5f, SCREEN_ASPECT, 0.1f, 1000.0f);
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
	BindShader(&gShaderGeometryBuffer);

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
	glViewport(0, 0, hw, hh);
	DrawColor(fGeometryBuffer, 0);

	// Normals (1, bottom-right)
	glViewport(hw, 0, hw, hh);
	DrawColor(fGeometryBuffer, 1);

	// Albedo (2, top-left
	glViewport(0, hh, hw, hh);
	DrawColor(fGeometryBuffer, 2);

	// Final render or light buffer top-right!?
	glViewport(hw, hh, hw, hh);

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}
