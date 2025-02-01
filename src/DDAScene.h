#pragma once
#include "SoftwareRenderingScene.h"

class DDAScene : public SoftwareRenderingScene
{
public:
	void OnUpdate(float dt) final;
};
