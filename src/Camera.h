#pragma once
#include "Math.h"

// FPS camera representation
struct FpsTransform
{
	Vector3 position = V3_ZERO;
	float pitch = 0.0f;
	float yaw = 0.0f;
};

// Camera update assumes delta-time has been applied to CameraDelta's values 
struct FpsDelta
{
	float pitch = 0.0f;
	float yaw = 0.0f;
	float right = 0.0f;
	float up = 0.0f;
	float forward = 0.0f;
};

// FPS camera update
RMAPI Matrix UpdateFpsCamera(FpsTransform& camera, FpsDelta delta)
{
	camera.yaw += delta.yaw;
	camera.pitch += delta.pitch;

	Matrix orientation = ToMatrix(FromEuler(camera.pitch, camera.yaw, 0.0f));
	camera.position += Right(orientation) * delta.right;
	camera.position += Up(orientation) * delta.up;
	camera.position += Forward(orientation) * delta.forward;

	Matrix view = Invert(orientation * Translate(camera.position));
	return view;
}

// So I don't need to copy-paste key-input everywhere xD
Matrix UpdateFpsCameraDefault(FpsTransform& camera, float dt);

extern FpsTransform gCamera;