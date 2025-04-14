#pragma once
#include "SoftwareRenderingScene.h"

class RasterizationScene : public SoftwareRenderingScene
{
public:
	void OnLoad() final;
	void OnUnload() final;

	void OnUpdate(float dt) final;
};
