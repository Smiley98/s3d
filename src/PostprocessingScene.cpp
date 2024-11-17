#include "PostprocessingScene.h"
#include "Window.h"
#include "Time.h"
#include "Render.h"
#include "Camera.h"

void PostprocessingScene::OnLoad()
{
	gCamera.position = { 0.0f, 0.0f, 5.0f };
}

void PostprocessingScene::OnUnload()
{
}

void PostprocessingScene::OnUpdate(float dt)
{
	UpdateFpsCameraDefault(gCamera, dt);

	// Raymarching uses the camera matrix, so no need to calculate the view matrix!
	//gView = ToView(gCamera);
}

void PostprocessingScene::OnDraw()
{
	float time = TotalTime();
	Vector2 resolution{ SCREEN_WIDTH, SCREEN_HEIGHT };
	Vector2 mouse = GetMousePosition() / resolution;
	mouse.y = 1.0f - mouse.y;
	mouse = mouse * 2.0f - 1.0f;

	Shader* shaders[]{ &gShaderRaymarchBase, &gShaderFractal2D, &gShaderFractal3D, &gShaderRaymarchTheft };
	static int shader = 0;
	if (IsKeyPressed(KEY_TAB))
		++shader %= 4;

	Matrix rot = FpsRotation(gCamera);
	BindShader(shaders[shader]);
	SendMat3("u_camRot", rot);
	SendVec3("u_camPos", gCamera.position);
	SendFloat("u_fov", tanf(90.0f * 0.5f * DEG2RAD));
	// Ensure the ray FoV of 90 degrees matches the perspective projection's FoV!
	
	SendVec2("u_resolution", resolution);
	SendVec2("u_mouse", mouse);
	SendFloat("u_time", time);
	DrawFsq();
	UnbindShader();


}

void PostprocessingScene::OnDrawImGui()
{
}
