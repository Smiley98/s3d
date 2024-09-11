#pragma once
#include "Scene.h"
#include "Render.h"

class RasterizationScene : public Scene
{
public:
	void OnDraw() final;
	void OnDrawImGui() final;
};
