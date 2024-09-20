#include "Scene.h"
class PostprocessingScene : public Scene
{
public:
	void OnCreate() final;
	void OnDestroy() final;

	void OnDraw() final;
	void OnDrawImGui() final;
};