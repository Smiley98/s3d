#pragma once
#include "Scene.h"
#include "Image.h"
#include "Texture.h"
#include <array>

constexpr size_t MAP_SIZE = 16;
constexpr size_t IMAGE_SIZE = 512;
constexpr size_t TILE_SIZE = IMAGE_SIZE / MAP_SIZE;

struct Cell
{
	int row = -1;
	int col = -1;
};

using Cells = std::vector<Cell>;

enum TileType : size_t
{
	AIR,
	WALL
};

class RaycastingScene : public Scene
{
public:
	void OnLoad() final;
	void OnUnload() final;

	void OnUpdate(float dt) final;
	void OnDraw() final;

private:
	Cells DDA(int x0, int y0, int x1, int y1);
	void DrawDDA(Vector2 start, Vector2 end);

	float mMoveSpeed = 250.0f;
	float mTurnSpeed = 360.0f;

	Vector2 mPosition;
	Vector2 mDirection{ 1.0f, 0.0f };

	Image mImage;
	GLuint mTexture;

	std::array<std::array<size_t, MAP_SIZE>, MAP_SIZE> mMap
	{
		std::array<size_t, MAP_SIZE>{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 },
		std::array<size_t, MAP_SIZE>{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		std::array<size_t, MAP_SIZE>{ 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		std::array<size_t, MAP_SIZE>{ 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		std::array<size_t, MAP_SIZE>{ 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		std::array<size_t, MAP_SIZE>{ 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		std::array<size_t, MAP_SIZE>{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		std::array<size_t, MAP_SIZE>{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		std::array<size_t, MAP_SIZE>{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		std::array<size_t, MAP_SIZE>{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		std::array<size_t, MAP_SIZE>{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		std::array<size_t, MAP_SIZE>{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		std::array<size_t, MAP_SIZE>{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		std::array<size_t, MAP_SIZE>{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		std::array<size_t, MAP_SIZE>{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 },
		std::array<size_t, MAP_SIZE>{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }
	};
};
