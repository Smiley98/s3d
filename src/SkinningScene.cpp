#include "SkinningScene.h"
#include "Window.h"
#include "Time.h"
#include "Render.h"
#include "Camera.h"
#include "ImageLoader.h"
#include <imgui/imgui.h>

// gltf loading is not for the weak...
void SkinningScene::OnLoad()
{
	gCamera = FromView(LookAt({ 0.0f, 15.0f, 30.0f }, V3_ZERO, V3_UP));
}

void SkinningScene::OnUnload()
{

}

void SkinningScene::OnUpdate(float dt)
{
	UpdateFpsCameraDefault(gCamera, dt);
	gView = ToView(gCamera);
	gProj = Perspective(90.0f * DEG2RAD, SCREEN_ASPECT, 0.1f, 100.0f);
}

void SkinningScene::OnDraw()
{
	DrawSphere(V3_ZERO, 10.0f, V3_RIGHT);
}
