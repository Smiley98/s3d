#include "RaycastingScene.h"
#include "Mesh.h"
#include "Shader.h"
#include "Window.h"

inline void ImageToTile(int ix, int iy, int& tx, int& ty)
{
	tx = ix / TILE_SIZE;
	ty = iy / TILE_SIZE;
}

inline void TileToImage(int tx, int ty, int& ix, int& iy)
{
	ix = tx * TILE_SIZE;
	iy = ty * TILE_SIZE;
}

void RaycastingScene::OnLoad()
{
	// Not sure if I want my raycaster's origin to be [0, 0] = top-left,
	// but that's how I've done it historically so I should be fine!
	LoadImage(mImage, IMAGE_SIZE, IMAGE_SIZE);
	for (size_t row = 0; row < MAP_SIZE; row++)
	{
		for (size_t col = 0; col < MAP_SIZE; col++)
		{
			int rx, ry;
			TileToImage(col, row, rx, ry);

			Color color = mMap[row][col] == WALL ? MAGENTA : BLACK;
			DrawRect(mImage, rx, ry, TILE_SIZE, TILE_SIZE, color);
		}
	}

	int x = 128;
	int y = 384;
	int r = 16;
	DrawCircle(		mImage, x, y, r, RED);
	DrawCircleLines(mImage, x, y, r, MAGENTA);

	int w, h;
	w = h = 32;
	x = 384;
	DrawRect(		mImage, x, y, w, h, RED);
	DrawRectLines(	mImage, x, y, w, h, MAGENTA);

	// Flip since array [0, 0] = top-left but uv [0, 0] = bottom-left
	Flip(mImage);
	mTexture = LoadTexture(mImage);
}

void RaycastingScene::OnUnload()
{
}

void RaycastingScene::OnUpdate(float dt)
{

}

void RaycastingScene::OnDraw()
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
