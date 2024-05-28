#include "MainScene.h"
#include "Shader.h"
#include "Mesh.h"
#include "App.h"

void MainScene::OnLoad()
{
	LoadImage(mImage, 512, 512);
	mTexture = LoadTexture(mImage);
}

void MainScene::OnUnload()
{
	UnloadTexture(mTexture);
	UnloadImage(mImage);
}

void MainScene::OnUpdate(float dt)
{
	// TODO -- render sphere's lines on the CPU
	for (size_t i = 0; i < gMeshHead.vertexCount; i += 3)
	{
		for (size_t j = 0; j < 3; j++)
		{
			Vector3 v0 = gMeshHead.positions[i + j];
			Vector3 v1 = gMeshHead.positions[i + ((j + 1) % 3)];
			v0.x = Remap(v0.x, -1.0f, 1.0f, 0, mImage.width  - 1);
			v0.y = Remap(v0.y, -1.0f, 1.0f, 0, mImage.height - 1);
			v1.x = Remap(v1.x, -1.0f, 1.0f, 0, mImage.width  - 1);
			v1.y = Remap(v1.y, -1.0f, 1.0f, 0, mImage.height - 1);

			int x0 = v0.x;
			int y0 = v0.y;
			int x1 = v1.x;
			int y1 = v1.y;
			DrawLine(mImage, x0, y0, x1, y1, GREEN);
		}
	}

	Vector3 v0{ 400.0f, 400.0f, 0.0f };
	Vector3 v1{ 400.0f, 200.0f, 0.0f };
	Vector3 v2{ 100.0, 100.0f, 0.0f };
	DrawTriangle(mImage, v0, v1, v2, RED);
}

void MainScene::OnDraw()
{
	UpdateTexture(mTexture, mImage);

	BindTexture(mTexture);
	BindShader(gShaderFSQ);
	glUniform1i(glGetUniformLocation(gShaderFSQ, "u_tex"), 0);
	BindFsq();
	DrawFsq();
	UnbindShader();
	UnbindTexture(mTexture);
}

void MainScene::OnDrawGui()
{
}
