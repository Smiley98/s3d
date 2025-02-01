#pragma once
#include "SoftwareRenderingScene.h"

class TestScene : public SoftwareRenderingScene
{
public:
	void OnUpdate(float dt) final;
};
