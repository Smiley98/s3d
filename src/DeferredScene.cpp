#include "DeferredScene.h"
#include "Render.h"

static Framebuffer fGeometryBuffer;

void DeferredScene::OnLoad()
{
	gCamera = FromView(LookAt({ 0.0f, 0.0f, 50.0f }, V3_ZERO, V3_UP));

	CreateFramebuffer(&fGeometryBuffer, SCREEN_WIDTH, SCREEN_HEIGHT);
	AddColor(&fGeometryBuffer, GL_RGBA16F, GL_RGBA, GL_FLOAT, GL_NEAREST);			// Positions
	AddColor(&fGeometryBuffer, GL_RGBA16F, GL_RGBA, GL_FLOAT, GL_NEAREST);			// Normals
	AddColor(&fGeometryBuffer, GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE, GL_NEAREST);		// Color
	AddDepth(&fGeometryBuffer);
	// Currently all shaders do texture(u_tex, uv).rgb so no sense in storing more than 3 texture channels
}

void DeferredScene::OnUnload()
{
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
	CreateMesh(&mesh, MESH_SPHERE);
	Matrix world = Scale(V3_ONE * 10.0f);
	Matrix mvp = world * gView * gProj;

	BindFramebuffer(fGeometryBuffer);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	BindShader(&gShaderGeometryBuffer);

	SendMat4("u_mvp", mvp);
	SendMat4("u_world", world);
	SendMat3("u_normal", NormalMatrix(world));
	DrawMesh(mesh);

	UnbindShader();
	UnbindFramebuffer();

	DestroyMesh(&mesh);

	// This could actually be achieved with a single texture unit since its 2 draw calls xD
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT * 0.5f);
	DrawColor(fGeometryBuffer, 0);
	glViewport(0, SCREEN_HEIGHT * 0.5f, SCREEN_WIDTH, SCREEN_HEIGHT * 0.5f);
	DrawColor(fGeometryBuffer, 1);
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}
