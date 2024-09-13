#pragma once
#include "Config.h"
#include "Math.h"

class Scene
{
public:

	enum Type : size_t
	{
		MAIN,
		TEST,
		DDA_TEST,
		RASTER,
		RAYCAST,
		COLLISION,
		COUNT
	};

	virtual void OnCreate() {}
	virtual void OnDestroy() {}

	virtual void OnLoad() {}
	virtual void OnUnload() {}

	virtual void OnUpdate(float dt) {}
	virtual void OnDraw() {}

	virtual void OnDrawGui() {}
	virtual void OnDrawImGui() {}

	static void Change(Type scene);
	static Type Current();

	static void Create(Type scene);
	static void Destroy();

	static void Update(float dt);
	static void Draw();
	static void DrawGui();
	static void DrawImGui();

private:
	static Scene* sScenes[COUNT];
	static Scene* sScene;
	static Type sCurrent;
	// Could use current in place of scene, but that would reduce legibility.
};
