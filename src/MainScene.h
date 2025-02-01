#pragma once
#include "SoftwareRenderingScene.h"

class MainScene : public SoftwareRenderingScene
{
public:
	void OnLoad() final;
	void OnUnload() final;

	void OnUpdate(float dt) final;
};
