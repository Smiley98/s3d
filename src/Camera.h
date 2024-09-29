#pragma once
#include "Math.h"

struct Camera
{
	Vector3 position = V3_ZERO;
	float pitch = 0.0f;
	float yaw = 0.0f;

	// Read-only, updated internally
	Matrix view = MatrixIdentity();
};

// Camera update assumes delta-time has been applied to CameraDelta's values 
struct CameraDelta
{
	float pitch = 0.0f;
	float yaw = 0.0f;
	float right = 0.0f;
	float up = 0.0f;
	float forward = 0.0f;
};

RMAPI void UpdateCamera(Camera& camera, CameraDelta delta)
{
	camera.yaw += delta.yaw;
	camera.pitch += delta.pitch;

	Matrix orientation = ToMatrix(FromEuler(camera.pitch, camera.yaw, 0.0f));
	camera.position += Right(orientation) * delta.right;
	camera.position += Up(orientation) * delta.up;
	camera.position += Forward(orientation) * delta.forward;

	camera.view = Invert(orientation * Translate(camera.position));
}
