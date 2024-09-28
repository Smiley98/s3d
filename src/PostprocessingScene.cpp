#include "PostprocessingScene.h"
#include "Window.h"
#include "Render.h"
#include "Time.h"

void PostprocessingScene::OnCreate()
{
	// Gives 3d fractal infinite rotation!
	SetMouseEnabled(false);
}

void PostprocessingScene::OnDestroy()
{
}

void PostprocessingScene::OnDraw()
{
	// Rotate based on direction from screen centre to mouse cursor
	Vector2 resolution{ SCREEN_WIDTH, SCREEN_HEIGHT };
	Vector2 mouse = MousePosition() / resolution;
	mouse.y = 1.0f - mouse.y;
	mouse = mouse * 2.0f - 1.0f;

	static bool f2d = true;
	if (IsKeyPressed(KEY_SPACE))
		f2d = !f2d;

	float time = TotalTime();
	BindShader(f2d ? &gShaderFractal2D : &gShaderFractal3D);

	// TODO -- recreate hybrid rendered planet scene!!!
	//BindShader(&gShaderRaymarchBase);
	//SendMat3("u_camRot", &invView);
	//SendVec3("u_camPos", cameraPosition);

	SendVec2("u_resolution", resolution);
	SendVec2("u_mouse", mouse);
	SendFloat("u_time", time);
	DrawFsq();
	UnbindShader();
}

void PostprocessingScene::OnDrawImGui()
{
}
