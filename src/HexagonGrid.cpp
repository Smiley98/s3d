#include "HexagonGrid.h"
#include "Render.h"
#include <cassert>

void GenHexagonGrid(HexagonGrid* grid, int rows, int cols, float r)
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

void DrawHexagonGridRaster(const HexagonGrid& grid)
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

void DrawHexagonGridDistance(Vector3 fg, Vector3 bg, float amount, float thickness)
{
	BindFramebuffer(gFboColor);
	BindShader(&gShaderHexagonGridDistance);
	SendVec2("u_resolution", { SCREEN_WIDTH, SCREEN_HEIGHT });
	SendVec3("u_fg_col", fg);
	SendVec3("u_bg_col", bg);
	SendFloat("u_hex_res", amount);
	SendFloat("u_hex_thickness", thickness);
	DrawFsq();
	UnbindShader();
	UnbindFramebuffer(gFboColor);

	DrawMeshTexture(gMeshPlane, Scale(100.0f * SCREEN_ASPECT, 100.0f, 1.0f), gFboColor.colors[0], 0);
}

// Hexagon Grid Coordinates Example:
//DrawMeshFlat(gMeshHexagon, Translate(0.0f, 0.0f, 0.0f), V3_ONE);
//DrawMeshFlat(gMeshHexagon, Translate(0.0f, sqrtf(3.0f), 0.0f), V3_ONE);
//DrawMeshFlat(gMeshHexagon, Translate(1.5f, sqrtf(3.0f) * 0.5f, 0.0f), V3_ONE);
//DrawMeshFlat(gMeshHexagon, Translate(3.0f, 0.0f, 0.0f), V3_ONE);
