#pragma once
#include "SoftwareRenderingScene.h"

class EffectScene : public SoftwareRenderingScene
{
public:
	void OnUpdate(float dt) final;
};
