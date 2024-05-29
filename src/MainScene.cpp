#include "MainScene.h"
#include "Shader.h"
#include "App.h"
#include "Rasterization.h"

void MainScene::OnLoad()
{
	LoadImage(mImage, 512, 512);
	LoadTexture(&mTexture, 512, 512);

	mMesh = gMeshSphere;
	mColors.resize(mMesh.vertexCount / 3);
	for (size_t i = 0; i < mColors.size(); i++)
		mColors[i] = RandomColor();
}

void MainScene::OnUnload()
{
	UnloadTexture(&mTexture);
	UnloadImage(mImage);
}

void MainScene::OnUpdate(float dt)
{
	//DrawMesh(mImage, mMesh, GREEN);
	//DrawMeshWireframes(mImage, mMesh, GREEN);

	for (size_t i = 0, c = 0; i < mMesh.vertexCount; i += 3, c++)
		DrawFace(mImage, mMesh, i, mColors[c]);
	
	for (size_t i = 0, c = 0; i < mMesh.vertexCount; i += 3, c++)
		DrawFaceWireframes(mImage, mMesh, i, mColors[c]);
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
