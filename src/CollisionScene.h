#pragma once
#include "Scene.h"

class CollisionScene : public Scene
{
public:
	void OnCreate() final;
	void OnDestroy() final;

	void OnUpdate(float dt) final;
	void OnDraw() final;
	void OnDrawImGui() final;
};
