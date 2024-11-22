#pragma once
#include "Scene.h"
class SolarSystemScene : public Scene
{
public:
	void OnLoad() final;
	void OnUnload() final;

	void OnUpdate(float dt) final;
	void OnDraw() final;
};
