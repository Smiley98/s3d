#include "MainScene.h"
#include "Shader.h"
#include "App.h"

void MainScene::OnLoad()
{
	LoadImage(mImage, 512, 512);
	mTexture = LoadTexture(mImage);

	mMesh = gMeshSphere;
	mColors.resize(mMesh.vertexCount / 3);
	for (size_t i = 0; i < mColors.size(); i++)
		mColors[i] = RandomColor();
}

void MainScene::OnUnload()
{
	UnloadTexture(mTexture);
	UnloadImage(mImage);
}

void MainScene::OnUpdate(float dt)
{
	// TODO -- render sphere's lines on the CPU
	for (size_t i = 0, c = 0; i < mMesh.vertexCount; i += 3, c++)
	{
		Vector3 vertices[3];
		for (size_t j = 0; j < 3; j++)
		{
			Vector3 v0 = mMesh.positions[i + j];
			v0.x = Remap(v0.x, -1.0f, 1.0f, 0, mImage.width  - 1);
			v0.y = Remap(v0.y, -1.0f, 1.0f, 0, mImage.height - 1);
			vertices[j] = v0;

			//Vector3 v1 = mMesh.positions[i + ((j + 1) % 3)];
			//v1.x = Remap(v1.x, -1.0f, 1.0f, 0, mImage.width  - 1);
			//v1.y = Remap(v1.y, -1.0f, 1.0f, 0, mImage.height - 1);

			//int x0 = v0.x;
			//int y0 = v0.y;
			//int x1 = v1.x;
			//int y1 = v1.y;
			//DrawLine(mImage, x0, y0, x1, y1, GREEN);
		}
		DrawTriangle(mImage, vertices[0], vertices[1], vertices[2], mColors[c]);
	}
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
