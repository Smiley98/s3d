#include "HexagonGridScene.h"
#include "Window.h"
#include "Render.h"
#include <vector>
#include <cassert>
#include <imgui/imgui.h>

struct HexagonGrid
{
	int rows = 0;
	int cols = 0;
	float radius = 1.0f;
	std::vector<std::vector<int>> values;
};

static HexagonGrid fGrid;
static bool fRasterize = true;

static float fHexRes = 10.0f;
static float fHexThickness = 0.1f;

static Framebuffer fFbo;

void GenGrid(HexagonGrid* grid, int rows, int cols, float r);
void DrawGrid(const HexagonGrid& grid);

void HexagonGridScene::OnLoad()
{
	GenGrid(&fGrid, 16, 32, 1.0f);
	gCamera = FromView(LookAt({ 0.0f, 0.0f, 50.0f }, V3_ZERO, V3_UP));

	CreateFramebuffer(&fFbo, SCREEN_WIDTH, SCREEN_HEIGHT);
	AddColor(&fFbo, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, GL_NEAREST);
	CompleteFramebuffer(&fFbo);
}

void HexagonGridScene::OnUnload()
{
	DestroyFramebuffer(&fFbo);
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
	bool depthTest = DepthTest();
	bool depthWrite = DepthWrite();
	SetDepthTest(false);
	SetDepthWrite(false);
	if (fRasterize)
	{
		SetWireframes(true);
		DrawGrid(fGrid);
		SetWireframes(false);
	}
	else
	{
		BindFramebuffer(fFbo);
		BindShader(&gShaderHexagonGridRaymarch);
		SendVec2("u_resolution", { SCREEN_WIDTH, SCREEN_HEIGHT });
		SendVec3("u_fg_col", { 0.8f, 0.85f, 1.0f });
		SendVec3("u_bg_col", V3_ONE);
		SendFloat("u_hex_res", fHexRes);
		SendFloat("u_hex_thickness", fHexThickness);
		DrawFsq();
		UnbindShader();
		UnbindFramebuffer(fFbo);

		DrawMeshTexture(gMeshPlane, Scale(100.0f * SCREEN_ASPECT, 100.0f, 1.0f), fFbo.colors[0], 0);
	}
	SetDepthWrite(depthWrite);
	SetDepthTest(depthTest);
	
	DrawMeshNormals(gMeshTd, Translate(V3_RIGHT * -40.0f * SCREEN_ASPECT + V3_UP * -40.0f), MatrixIdentity());
}

void HexagonGridScene::OnDrawImGui()
{
	if (!fRasterize)
	{
		ImGui::SliderFloat("Hexagon Resolution", &fHexRes, 1.0f, 100.0f);
		ImGui::SliderFloat("Hexagon Thickness", &fHexThickness, 0.1f, 0.9f);
	}
}

void GenGrid(HexagonGrid* grid, int rows, int cols, float r)
{
	assert(rows > 0 && cols > 0);
	grid->rows = rows;
	grid->cols = cols;
	grid->radius = r;
	grid->values.resize(rows);
	for (int i = 0; i < rows; i++)
		grid->values[i].resize(cols);

	for (int row = 0; row < rows; row++)
	{
		grid->values[row][0] = 1;
		grid->values[row][cols - 1] = 1;
	}

	for (int col = 0; col < cols; col++)
	{
		grid->values[0][col] = 1;
		grid->values[rows - 1][col] = 1;
	}
}

void DrawGrid(const HexagonGrid& grid)
{
	for (int i = 0; i < grid.rows; i++)
	{
		for (int j = 0; j < grid.cols; j++)
		{
			Vector3 color = grid.values[i][j] == 0 ? V3_RIGHT : V3_UP;
			float r = grid.radius;
			float sc = 1.5f * r;		// Space between columns
			float sr = sqrtf(3.0f) * r;	// Space between rows
			float offset = j % 2 == 0 ? 0.0f : sr * 0.5f;
			DrawMeshFlat(gMeshHexagon, Scale(r, r, 1.0f) * Translate(j * sc, i * sr + offset, 0.0f), color);
		}
	}
}

// Hexagon Grid Coordinates Example:
//DrawMeshFlat(gMeshHexagon, Translate(0.0f, 0.0f, 0.0f), V3_ONE);
//DrawMeshFlat(gMeshHexagon, Translate(0.0f, sqrtf(3.0f), 0.0f), V3_ONE);
//DrawMeshFlat(gMeshHexagon, Translate(1.5f, sqrtf(3.0f) * 0.5f, 0.0f), V3_ONE);
//DrawMeshFlat(gMeshHexagon, Translate(3.0f, 0.0f, 0.0f), V3_ONE);
