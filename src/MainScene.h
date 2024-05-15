#pragma once
#include "Scene.h"

class MainScene : public Scene
{
public:
	void OnUpdate(float dt) final;
	void OnDraw() final;

	void OnDrawGui() final;
};