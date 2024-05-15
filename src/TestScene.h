#pragma once
#include "Scene.h"

class TestScene : public Scene
{
public:
	void OnUpdate(float dt) final;
	void OnDraw() final;
};
