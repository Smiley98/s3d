#pragma once
#include "Scene.h"
#include "Image.h"
#include "Texture.h"
#include <array>

class RaycastingScene : public Scene
{
public:
	void OnLoad() final;
	void OnUnload() final;

	void OnUpdate(float dt) final;
	void OnDraw() final;

private:
	struct RayHit
	{
		float t;	// Distance along ray
		size_t i;	// Index of tile hit (tile type)
		//Vector2 poi; poi = position + direction * t
	};

	RayHit Raycast(Vector2 position, Vector2 direction);
	std::vector<RayHit> mHits;

	float mMoveSpeed = 250.0f;
	float mTurnSpeed = 360.0f;

	Vector2 mPosition;
	Vector2 mDirection{ 1.0f, 0.0f };

	Image mImage;
	Texture mTexture;

	// 0 = air, 1 = red, 2 = green, 3 = blue
	std::array<std::array<size_t, 16>, 16> mMap
	{
		std::array<size_t, 16>{ 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 },
		std::array<size_t, 16>{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2 },
		std::array<size_t, 16>{ 1, 0, 2, 0, 2, 2, 2, 0, 0, 0, 1, 1, 1, 3, 0, 2 },
		std::array<size_t, 16>{ 1, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 3, 0, 2 },
		std::array<size_t, 16>{ 1, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 3, 0, 2 },
		std::array<size_t, 16>{ 1, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 3, 0, 2 },
		std::array<size_t, 16>{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 2 },
		std::array<size_t, 16>{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 2 },
		std::array<size_t, 16>{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 3, 0, 2 },
		std::array<size_t, 16>{ 1, 0, 2, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 2 },
		std::array<size_t, 16>{ 1, 0, 2, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 2 },
		std::array<size_t, 16>{ 1, 0, 2, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 2 },
		std::array<size_t, 16>{ 1, 0, 2, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 2 },
		std::array<size_t, 16>{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 2 },
		std::array<size_t, 16>{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2 },
		std::array<size_t, 16>{ 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 }
	};
};
