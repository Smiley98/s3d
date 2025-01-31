#include "HexagonGridScene.h"
#include "Window.h"
#include "Render.h"
#include "HexagonGrid.h"

#include <cassert>
#include <imgui/imgui.h>

static bool fRasterize = true;
static float fHexagonAmount = 10.0f;
static float fHexagonThickness = 0.1f;
static HexagonGrid fGrid;

void HexagonGridScene::OnLoad()
{
	GenHexagonGrid(&fGrid, 16, 32, 1.0f);
	gCamera = FromView(LookAt({ 0.0f, 0.0f, 50.0f }, V3_ZERO, V3_UP));
}

void HexagonGridScene::OnUnload()
{
}

void HexagonGridScene::OnUpdate(float dt)
{
	UpdateFpsCameraDefault(gCamera, dt);
	gView = ToView(gCamera);
	gProj = Perspective(PI * 0.5f, SCREEN_ASPECT, 0.1f, 1000.0f);

	if (IsKeyPressed(KEY_R))
		fRasterize = !fRasterize;
}

void HexagonGridScene::OnDraw()
{
	if (fRasterize)
	{
		SetPipelineState(gPipelineWireframes);
		DrawHexagonGridRaster(fGrid);
		SetPipelineState(gPipelineDefault);
	}
	else
	{

		SetPipelineState(gPipelineFsq);
		DrawHexagonGridDistance({ 0.8f, 0.85f, 1.0f }, V3_ONE, fHexagonAmount, fHexagonThickness);
		SetPipelineState(gPipelineDefault);
	}
	
	DrawMeshNormals(gMeshTd, Translate(V3_RIGHT * -40.0f * SCREEN_ASPECT + V3_UP * -40.0f), MatrixIdentity());
}

void HexagonGridScene::OnDrawImGui()
{
	if (!fRasterize)
	{
		ImGui::SliderFloat("Hexagon Amount", &fHexagonAmount, 1.0f, 100.0f);
		ImGui::SliderFloat("Hexagon Thickness", &fHexagonThickness, 0.1f, 0.9f);
	}
}
