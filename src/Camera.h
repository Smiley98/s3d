#pragma once
#include "Math.h"

// FPS camera representation
struct FpsTransform
{
	Vector3 position = V3_ZERO;
	float pitch = 0.0f;
	float yaw = 0.0f;
};

extern FpsTransform gCamera;

// Camera update assumes delta-time has been applied to CameraDelta's values 
struct FpsDelta
{
	float pitch = 0.0f;
	float yaw = 0.0f;
	float right = 0.0f;
	float up = 0.0f;
	float forward = 0.0f;
};

// Get camera rotation matrix from pitch and yaw (rotate about y, then rotate about x)
inline Matrix FpsRotation(FpsTransform camera)
{
	return ToMatrix(FromEuler(camera.pitch, camera.yaw, 0.0f));
}

// FPS camera update
inline void UpdateFpsCamera(FpsTransform& camera, FpsDelta delta)
{
	camera.yaw += delta.yaw;
	camera.pitch += delta.pitch;

	Matrix rotation = FpsRotation(camera);
	camera.position += Right(rotation) * delta.right;
	camera.position += Up(rotation) * delta.up;
	camera.position += Forward(rotation) * delta.forward;
}

// FPS camera update based on defualt logic
void UpdateFpsCameraDefault(FpsTransform& camera, float dt);

// From camera matrix to view matrix
inline Matrix ToView(FpsTransform camera)
{
	Matrix translation = Translate(camera.position);
	Matrix rotation = FpsRotation(camera);
	return Invert(rotation * translation);
}

// From view matrix to camera matrix
inline FpsTransform FromView(Matrix view)
{
	FpsTransform transform;
	Matrix camera = Invert(view);

	Vector3 euler = ToEuler(FromMatrix(camera));
	transform.pitch = euler.x;
	transform.yaw = euler.y;

	transform.position = Translation(camera);
	return transform;
}
