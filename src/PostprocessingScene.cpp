#include "PostprocessingScene.h"
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
	float time = TotalTime();
	Vector2 resolution{ SCREEN_WIDTH, SCREEN_HEIGHT };
	BindShader(&gShaderFractal3D);
	SendVec2("u_resolution", resolution);
	SendFloat("u_time", time);
	DrawFsq();
	UnbindShader();
}

void PostprocessingScene::OnDrawImGui()
{
}
