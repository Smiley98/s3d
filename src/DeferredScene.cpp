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

	// When sampling from the g-buffer, we need bind positions normals and colours all at once (3 texture units).
	// However if only visualizing one at a time, then only 1 texture unit is needed
	// TODO -- Add a slot to UnbindTexture and give each texture a slot value. Check if slot parameter matches texture's slot.

	DrawColor(fGeometryBuffer);
	//DrawColor(fGeometryBuffer, 1);
	// TODO -- fix texture slots - normals don't render as attechment 1 but they render as attachment 0...
}
