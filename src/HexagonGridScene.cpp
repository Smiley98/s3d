#include "HexagonGridScene.h"
#include "Window.h"
#include "Render.h"
#include <vector>
#include <cassert>

struct HexagonGrid
{
	int rows = 0;
	int cols = 0;
	float radius = 1.0f;
	std::vector<std::vector<int>> values;
};

static HexagonGrid fGrid;
static bool fRasterize = true;

void GenGrid(HexagonGrid* grid, int rows, int cols, float r);
void DrawGrid(const HexagonGrid& grid);

void HexagonGridScene::OnLoad()
{
	GenGrid(&fGrid, 16, 32, 1.0f);
	gCamera = FromView(LookAt({ 0.0f, 0.0f, 5.0f }, V3_ZERO, V3_UP));
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
		bool depthTest = DepthTest();
		bool depthWrite = DepthWrite();
		SetWireframes(true);
		SetDepthTest(false);
		SetDepthWrite(false);
		DrawGrid(fGrid);
		SetDepthWrite(depthWrite);
		SetDepthTest(depthTest);
		SetWireframes(false);
	}
	else
	{
		// TODO -- render to texture, then sample from texture in rasterized plane
		BindShader(&gShaderHexagonGridRaymarch);
		SendVec2("u_resolution", { SCREEN_WIDTH, SCREEN_HEIGHT });
		SendVec3("fg_col", { 0.8f, 0.85f, 1.0f });
		SendVec3("bg_col", V3_ONE);
		DrawFsq();
		UnbindShader();
	}

	DrawMeshNormals(gMeshTd, MatrixIdentity(), MatrixIdentity());
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
