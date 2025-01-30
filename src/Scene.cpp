#include "Scene.h"
#include "MainScene.h"
#include "TestScene.h"
#include "DDAScene.h"
#include "RasterizationScene.h"
#include "RaycastingScene.h"
#include "CollisionScene.h"
#include "PhysicsScene.h"
#include "PostprocessingScene.h"
#include "SolarSystemScene.h"
#include "DeferredScene.h"
#include "NeonDrive.h"
#include "HexagonGridScene.h"

Scene* Scene::sScenes[Scene::COUNT]{};
Scene* Scene::sScene = nullptr;
Scene::Type Scene::sCurrent = Scene::COUNT;

void Scene::Create(Scene::Type scene)
{
	sScenes[MAIN] = new MainScene;
	sScenes[TEST] = new TestScene;
	sScenes[DDA_TEST] = new DDAScene;
	sScenes[RASTER] = new RasterizationScene;
	sScenes[RAYCAST] = new RaycastingScene;
	sScenes[COLLISION] = new CollisionScene;
	sScenes[PHYSICS] = new PhysicsScene;
	sScenes[POST_PROCESSING] = new PostprocessingScene;
	sScenes[SOLAR_SYSTEM] = new SolarSystemScene;
	sScenes[DEFERRED_RENDERING] = new DeferredScene;
	sScenes[NEON_DRIVE] = new NeonDriveScene;
	sScenes[HEXAGON_GRID] = new HexagonGridScene;

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
