#include "RaycastingScene.h"
#include "Mesh.h"
#include "Shader.h"

inline void ScreenToMap(int sx, int sy, int& mx, int& my)
{
	mx = sx / TILE_SIZE;
	my = sy / TILE_SIZE;
}

inline void MapToScreen(int mx, int my, int& sx, int& sy)
{
	sx = mx * TILE_SIZE;
	sy = my * TILE_SIZE;
}
void RaycastingScene::OnLoad()
{
	// Not sure if I want my raycaster's origin to be [0, 0] = top-left,
	// but that's how I've done it historically so I should be fine!
	LoadImage(mImage, SCREEN_SIZE, SCREEN_SIZE);
	for (size_t row = 0; row < MAP_SIZE; row++)
	{
		for (size_t col = 0; col < MAP_SIZE; col++)
		{
			int rx, ry;
			MapToScreen(col, row, rx, ry);

			Color color = mMap[row][col] == WALL ? MAGENTA : BLACK;
			DrawRect(mImage, rx, ry, TILE_SIZE, TILE_SIZE, color);
		}
	}

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
	glDepthMask(GL_FALSE);
	glUseProgram(gShaderFSQ);
	glUniform1i(glGetUniformLocation(gShaderFSQ, "u_tex"), 0);
	BindFsq();
	DrawFsq();

	glBindVertexArray(GL_NONE);
	glUseProgram(GL_NONE);
	glDepthMask(GL_TRUE);
	UnbindTexture(mTexture);
}
