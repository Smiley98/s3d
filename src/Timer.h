#pragma once
#include <cmath>

struct Timer
{
	float current = 0.0f;
	float total = 0.0f;

	void Tick(float dt)
	{
		current += dt;
	}

	void Reset()
	{
		current = 0.0f;
	}

	bool Expired()
	{
		return current >= total;
	}

	float Percent()
	{
		return fminf(current / total, 1.0f);
	}

	float PercentUnclamped()
	{
		return current / total;
	}
};
