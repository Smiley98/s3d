#pragma once
#include "Scene.h"
#include "Image.h"

extern Image gImageCPU;

class SoftwareRenderingScene : public Scene
{
	void OnDraw() final;
};
