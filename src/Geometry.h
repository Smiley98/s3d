#pragma once
#include "Math.h"

inline Vector3 RandomSpherePoint(float radius)
{
	float z = Random(-1.0f, 1.0f);
	float t = Random(0.0f, PI * 2.0);
	float r = sqrtf(1.0f - z * z);
	return Vector3{ r * cosf(t), r * sinf(t), z } *radius;
}

// TODO -- Add more generation functions here like generating within a cube or a frustum.