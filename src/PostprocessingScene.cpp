#include "PostprocessingScene.h"
#include "Window.h"
#include "Camera.h"
#include "Render.h"
#include "Time.h"

void PostprocessingScene::OnLoad()
{
	gCamera.position = { 0.0f, 0.0f, 5.0f };
}

void PostprocessingScene::OnUnload()
{
}

void PostprocessingScene::OnUpdate(float dt)
{
	// Note that raymarching doesn' use the view matrix!
	// This is being done mainly to update camera position and orientation.
	gView = UpdateFpsCameraDefault(gCamera, dt);
}

void PostprocessingScene::OnDraw()
{
	// TODO -- recreate hybrid rendered planet scene!!!
	static bool f2d = true;
	if (IsKeyPressed(KEY_SPACE))
		f2d = !f2d;

	float time = TotalTime();
	Vector2 resolution{ SCREEN_WIDTH, SCREEN_HEIGHT };
	Vector2 mouse = GetMousePosition() / resolution;
	mouse.y = 1.0f - mouse.y;
	mouse = mouse * 2.0f - 1.0f;

	// TODO -- Make 3d fractal use fps camera, or implement arcball!!
	//BindShader(f2d ? &gShaderFractal2D : &gShaderFractal3D);

	BindShader(&gShaderRaymarchBase);
	SendMat3("u_camRot", &gCamera.orientation);
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
