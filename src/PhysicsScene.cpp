#include "PhysicsScene.h"
#include "Render.h"
#include "Camera.h"

void PhysicsScene::OnLoad()
{
	gCamera = FromView(LookAt({ 0.0f, 0.0f, 5.0f }, V3_ZERO, V3_UP));
}

void PhysicsScene::OnUnload()
{
}

void PhysicsScene::OnUpdate(float dt)
{
	UpdateFpsCameraDefault(gCamera, dt);
	gView = ToView(gCamera);
	gProj = Ortho(-100.0f * SCREEN_ASPECT, 100.0f * SCREEN_ASPECT, -100.0f, 100.0f, 0.1f, 10.0f);
}

void PhysicsScene::OnDraw()
{
	DrawCircle({}, 10.0f, V3_UP);
}
