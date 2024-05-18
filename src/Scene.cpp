#include "Scene.h"
#include "MainScene.h"
#include "TestScene.h"
#include "RasterizationScene.h"

Scene* Scene::sScenes[Scene::COUNT]{};
Scene* Scene::sScene = nullptr;
Scene::Type Scene::sCurrent = Scene::COUNT;

void Scene::Create(Scene::Type scene)
{
	sScenes[TEST] = new TestScene;
	sScenes[MAIN] = new MainScene;
	sScenes[RASTER] = new RasterizationScene;

	for (size_t i = 0; i < COUNT; i++)
		sScenes[i]->OnCreate();

	sScene = sScenes[scene];
	sCurrent = scene;
	sScene->OnLoad();
}

void Scene::Destroy()
{
	sScene->OnUnload();
	for (size_t i = 0; i < COUNT; i++)
	{
		sScenes[i]->OnDestroy();
		delete sScenes[i];
		sScenes[i] = nullptr;
	}
	sScene = nullptr;
	sCurrent = COUNT;
}

void Scene::Change(Scene::Type scene)
{
	sScene->OnUnload();
	sScene = sScenes[scene];
	sCurrent = scene;
	sScene->OnLoad();
}

Scene::Type Scene::Current()
{
	return sCurrent;
}

void Scene::Update(float dt)
{
	sScene->OnUpdate(dt);
}

void Scene::Draw()
{
	sScene->OnDraw();
}

void Scene::DrawGui()
{
	sScene->OnDrawGui();
}

void Scene::DrawImGui()
{
	sScene->OnDrawImGui();
}
