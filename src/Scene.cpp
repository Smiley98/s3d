#include "Scene.h"
#include "Window.h"
#include <imgui/imgui.h>

#include "MainScene.h"
#include "TestScene.h"
#include "DDAScene.h"
#include "RasterizationScene.h"
#include "RaycastingScene.h"
#include "CollisionScene.h"
#include "PhysicsScene.h"
#include "PostprocessingScene.h"
#include "SolarSystemScene.h"
#include "NeonDrive.h"
#include "CarScene.h"

Scene* Scene::sScenes[Scene::COUNT]{};
Scene::Type Scene::sCurrent = Scene::COUNT;
bool Scene::sSelect = false;

void Scene::Create(Scene::Type scene)
{
	sScenes[SOLAR_SYSTEM] = new SolarSystemScene;
	sScenes[NEON_LIGHTS] = new NeonDriveScene;
	sScenes[REFLECTIVE_PAINT] = new CarScene;
	sScenes[FRACTALS] = new PostprocessingScene;

	sScenes[GRAPHICS_TEST] = new RasterizationScene;
	sScenes[PHYSICS_TEST] = new PhysicsScene;
	sScenes[COLLISION_TEST] = new CollisionScene;

	sScenes[CPU_RASTERIZATION] = new MainScene;
	sScenes[CPU_RAYCASTING] = new RaycastingScene;
	sScenes[CPU_EFFECT] = new TestScene;

	for (size_t i = 0; i < COUNT; i++)
		sScenes[i]->OnCreate();

	sCurrent = scene;
	sScenes[sCurrent]->OnLoad();
}

void Scene::Destroy()
{
	sScenes[sCurrent]->OnUnload();
	for (size_t i = 0; i < COUNT; i++)
	{
		sScenes[i]->OnDestroy();
		delete sScenes[i];
		sScenes[i] = nullptr;
	}
	sCurrent = COUNT;
}

void Scene::Change(Scene::Type scene)
{
	sScenes[sCurrent]->OnUnload();
	sCurrent = scene;
	sScenes[sCurrent]->OnLoad();
}

Scene::Type Scene::Current()
{
	return sCurrent;
}

void Scene::Update(float dt)
{
	sScenes[sCurrent]->OnUpdate(dt);
	if (IsKeyPressed(KEY_GRAVE_ACCENT))
		sSelect = !sSelect;
}

void Scene::Draw()
{
	sScenes[sCurrent]->OnDraw();
}

void Scene::DrawGui()
{
	sScenes[sCurrent]->OnDrawGui();
}

void Scene::DrawImGui()
{
	if (sSelect)
	{
		static const char* sceneNames[] =
		{
			"Solar System",
			"Neon Lights",
			"Reflective Paint",
			"Fractals",

			"Graphics Test",
			"Physics Test",
			"Collision Test",

			"CPU Rasterization",
			"CPU Raycasting",
			"CPU Effect"
		};

		static int scene = 0;
		if (ImGui::Combo("Scenes", &scene, sceneNames, IM_ARRAYSIZE(sceneNames)))
			Change((Type)scene);
	}
	sScenes[sCurrent]->OnDrawImGui();
}

// Old approach to scene-selection xD
//if (IsKeyPressed(KEY_GRAVE_ACCENT))
//{
//	int scene = Scene::Current();
//	++scene %= Scene::Type::COUNT;
//	Scene::Change((Scene::Type)scene);
//}
