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

void GenHexagonGridRaster(HexagonGrid* grid, int rows, int cols, float r);
void DrawHexagonGridRaster(const HexagonGrid& grid);
