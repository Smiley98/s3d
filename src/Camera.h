#pragma once
#include "Math.h"

struct Camera
{
	Vector3 position = V3_ZERO;
	Quaternion yaw = QuaternionIdentity();
	Quaternion pitch = QuaternionIdentity();

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

inline void UpdateCamera(Camera& camera, CameraDelta delta)
{
	camera.yaw = camera.yaw * FromEuler(0.0f, delta.yaw, 0.0f);
	camera.pitch = camera.pitch * FromEuler(delta.pitch, 0.0f, 0.0f);
	Quaternion rotation = camera.yaw * camera.pitch;

	Matrix orientation = ToMatrix(rotation);
	camera.position = camera.position + Right(orientation) * delta.right;
	camera.position = camera.position + Up(orientation) * delta.up;
	camera.position = camera.position + Forward(orientation) * delta.forward;

	DebugMatrix dbg = ToDebug(orientation);

	camera.view = Invert(orientation * Translate(camera.position));
}
