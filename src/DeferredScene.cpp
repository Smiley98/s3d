#include "DeferredScene.h"
#include "Render.h"
#include "Camera.h"

void DeferredScene::OnLoad()
{
	gCamera = FromView(LookAt({ 0.0f, 0.0f, 50.0f }, V3_ZERO, V3_UP));
}

void DeferredScene::OnUnload()
{
}

void DeferredScene::OnUpdate(float dt)
{
	UpdateFpsCameraDefault(gCamera, dt);
	gView = ToView(gCamera);
	gProj = Perspective(PI * 0.5f, SCREEN_ASPECT, 0.1f, 1000.0f);
}

void DeferredScene::OnDraw()
{
	DrawSphere({}, 25.0f, V3_RIGHT);
}
