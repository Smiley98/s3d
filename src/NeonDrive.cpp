#include "NeonDrive.h"
#include "Time.h"
#include "Render.h"
#include "ImageLoader.h"

static Texture2D fTextureWhite;
static Texture2D fTextureAlbedo;
static Texture2D fTextureGround;
static Framebuffer fGeometryBuffer;

static Vector3 fLightPosition = V3_ZERO;
static Vector3 fLightColor = V3_ONE;

void NeonDriveScene::OnLoad()
{
	gCamera = FromView(LookAt({ 20.0f, 20.0f, 20.0f }, V3_ZERO, V3_UP));

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

	// Original format = RGB
	{
		int w, h, c;
		uint8_t* pixels = LoadImage2D("./assets/textures/ground.png", &w, &h, &c);
		CreateTexture2D(&fTextureGround, w, h, GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE, GL_LINEAR, pixels);
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
	DestroyTexture2D(&fTextureGround);
	DestroyTexture2D(&fTextureAlbedo);
	DestroyTexture2D(&fTextureWhite);
}

void NeonDriveScene::OnUpdate(float dt)
{
	UpdateFpsCameraDefault(gCamera, dt);
	gView = ToView(gCamera);
	gProj = Perspective(PI * 0.5f, SCREEN_ASPECT, 0.1f, 1000.0f);

	float tt = TotalTime();
	fLightPosition = { cosf(tt) * 10.0f, 0.0f, 0.0f };
}

void NeonDriveScene::OnDraw()
{
	DrawMeshTexture(gMeshGround, Scale(100.0f, 1.0f, 100.0f), fTextureGround, 0);
	DrawMeshNormals(gMeshTd, MatrixIdentity(), MatrixIdentity());
	DrawMeshNormals(gMeshHexagon, Scale(V3_ONE * 5.0f) * Translate(0.0, 1.0f, 10.0f), MatrixIdentity());
	return;

	Matrix world = MatrixIdentity();//Scale(V3_ONE * 10.0f);
	Matrix mvp = world * gView * gProj;

	BindFramebuffer(fGeometryBuffer);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	BindTexture2D(fTextureWhite, 0);
	BindShader(&gShaderDeferred);

	SendMat4("u_mvp", mvp);
	SendMat4("u_world", world);
	SendMat3("u_normal", NormalMatrix(world));
	SendInt("u_tex", 0);
	DrawMesh(gMeshTd);

	UnbindShader();
	UnbindTexture2D(fTextureWhite, 0);
	UnbindFramebuffer(fGeometryBuffer);

	BindTexture2D(fGeometryBuffer.colors[0], 0);
	BindTexture2D(fGeometryBuffer.colors[1], 1);
	BindTexture2D(fGeometryBuffer.colors[2], 2);
	BindShader(&gShaderDeferredLighting);

	Matrix S = Scale(V3_ONE * 10.0f);
	Matrix R = LookRotation(fLightPosition, gCamera.position, V3_UP);
	Matrix T = Translate(fLightPosition);
	world = S * R * T;
	mvp = world * gView * gProj;
	SendMat4("u_mvp", mvp);
	SendInt("u_positions", 0);
	SendInt("u_normals", 1);
	SendInt("u_albedo", 2);
	SendVec3("u_lightPosition", fLightPosition);
	SendVec3("u_lightColor", fLightColor);
	SendFloat("u_ambient", 1.0f);
	SendFloat("u_diffuse", 1.0f);
	SendVec2("u_viewportSize", { SCREEN_WIDTH, SCREEN_HEIGHT });
	SendVec2("u_viewportOffset", V2_ZERO);

	bool depthTest = DepthTest();
	bool depthWrite = DepthWrite();
	SetDepthTest(false);
	SetDepthWrite(false);
	DrawMesh(gMeshCircle);
	SetDepthWrite(depthWrite);
	SetDepthTest(depthTest);

	UnbindShader();
	UnbindTexture2D(fGeometryBuffer.colors[2], 2);
	UnbindTexture2D(fGeometryBuffer.colors[1], 1);
	UnbindTexture2D(fGeometryBuffer.colors[0], 0);

	DrawMeshWireframes(gMeshCircle, world, fLightColor);
}
