#pragma once
#include "Math.h"
#include <vector>

struct HexagonGrid
{
	int rows = 0;
	int cols = 0;
	float radius = 1.0f;
	std::vector<std::vector<int>> values;
};

void GenHexagonGrid(HexagonGrid* grid, int rows, int cols, float r);

void DrawHexagonGridRaster(const HexagonGrid& grid);
void DrawHexagonGridDistance(Vector3 fg, Vector3 bg, float amount, float thickness);

// TODO -- Find a way to make rasterized vs SDF hex-grid line up?
// Not really relevant, probably only going to use SDF hex-grid.
// Would still be cool to seamlessly switch between the two though!
