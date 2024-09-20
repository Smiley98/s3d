#include "PostprocessingScene.h"
#include "Window.h"
#include "Render.h"
#include "Time.h"

void PostprocessingScene::OnCreate()
{
}

void PostprocessingScene::OnDestroy()
{
}

void PostprocessingScene::OnDraw()
{
	// TODO -- make FPS camera and send as matrix to GPU instead!
	Vector2 mouse = MouseDelta() * DEG2RAD;
	//printf("%f %f\n", mouse.x, mouse.y);

	static bool f2d = true;
	if (IsKeyPressed(KEY_SPACE))
		f2d = !f2d;

	float time = TotalTime();
	Vector2 resolution{ SCREEN_WIDTH, SCREEN_HEIGHT };
	BindShader(f2d ? &gShaderFractal2D : &gShaderFractal3D);
	SendVec2("u_resolution", resolution);
	SendVec2("u_mouse", mouse);
	SendFloat("u_time", time);
	DrawFsq();
	UnbindShader();
}

void PostprocessingScene::OnDrawImGui()
{
}
