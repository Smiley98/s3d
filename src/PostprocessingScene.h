#include "Scene.h"
class PostprocessingScene : public Scene
{
public:
	void OnLoad() final;
	void OnUnload() final;

	void OnDraw() final;
	void OnDrawImGui() final;
};