#include "DDAScene.h"
#include "Shader.h"
#include "Mesh.h"
#include "Window.h"

constexpr size_t IMAGE_SIZE = 512;
constexpr Vector2 CENTER{ IMAGE_SIZE * 0.5f, IMAGE_SIZE * 0.5f };

struct Cell
{
	int col = -1;
	int row = -1;
};
using Cells = std::vector<Cell>;

inline Cells DDA(int x0, int y0, int x1, int y1)
{
	Cells cells;

	int dx = x1 - x0;
	int dy = y1 - y0;

	int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);
	float xStep = dx / (float)steps;
	float yStep = dy / (float)steps;

	float x = x0;
	float y = y0;
	for (int i = 0; i < steps; i++)
	{
		x += xStep;
		y += yStep;

		Cell cell;
		cell.row = y;
		cell.col = x;
		cells.push_back(cell);
	}

	return cells;
}

void DDAScene::OnLoad()
{
	LoadImage(mImage, IMAGE_SIZE, IMAGE_SIZE);
	mTexture = LoadTexture(mImage);
}

void DDAScene::OnUnload()
{
	UnloadTexture(mTexture);
	UnloadImage(mImage);
}

void DDAScene::OnUpdate(float dt)
{
	for (int y = 0; y < mImage.height; y++)
	{
		for (int x = 0; x < mImage.width; x++)
		{
			Vector2 fragCoord{ x, y };
			Vector2 resolution{ mImage.width, mImage.height };
			Vector2 uv = fragCoord / resolution;
			Color color = Convert(uv);
			SetPixel(mImage, x, y, color);
		}
	}
	Flip(mImage);

	Vector2 mouse = MousePosition();
	mouse = ScreenToImage(mImage, mouse);
	int r = 3;

	Cells pixels = DDA(CENTER.x, CENTER.y, mouse.x, mouse.y);
	for (Cell cell : pixels)
	{
		SetPixel(mImage, cell.col, cell.row, CYAN);
	}

	// Make sure mouse doesn't go off-screen (prevent image array out of bounds)
	//bool bounded = RectRect(8, 8, 496, 496, mouse.x - r, mouse.y - r, r * 2, r * 2);
	//if (bounded)
	//{
	//	DrawLine(mImage, CENTER.x, CENTER.y, mouse.x, mouse.y, RED);
	//	DrawCircle(mImage, CENTER.x, CENTER.y, r, RED);
	//	DrawCircle(mImage, mouse.x, mouse.y, r, RED);
	//}

	Flip(mImage);
}

void DDAScene::OnDraw()
{
	UpdateTexture(mTexture, mImage);

	BindTexture(mTexture);
	glUseProgram(gShaderFSQ);
	glUniform1i(glGetUniformLocation(gShaderFSQ, "u_tex"), 0);
	BindFsq();
	DrawFsq();

	glBindVertexArray(GL_NONE);
	glUseProgram(GL_NONE);
	UnbindTexture(mTexture);
}
