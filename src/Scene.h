#pragma once
#include "Config.h"
#include "Math.h"

class Scene
{
public:

	enum Type : size_t
	{
		SOLAR_SYSTEM,
		NEON_LIGHTS,
		REFLECTIVE_PAINT,
		FRACTAL,
		//SKINNING,

		GRAPHICS_TEST,
		PHYSICS_TEST,
		COLLISION_TEST,

		CPU_RASTERIZATION,
		CPU_RAYCASTING,
		CPU_EFFECT,

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
	static Type sCurrent;
	static bool sSelect;
	// Could use current in place of scene, but that would reduce legibility.
};
