#include "Camera.h"
#include "Window.h"
#include <array>

FpsTransform gCamera;

Matrix UpdateFpsCameraDefault(FpsTransform& camera, float dt)
{
	if (IsKeyPressed(KEY_C))
		SetMouseState(GetMouseState() != MOUSE_STATE_NORMAL ? MOUSE_STATE_NORMAL : MOUSE_STATE_DISABLED);

	Vector2 mouseDelta = GetMouseState() != MOUSE_STATE_NORMAL ? GetMouseDelta() : V2_ZERO;
	FpsDelta cameraDelta;
	float cameraLinearSpeed = dt * 10.0f; //2.5f;
	float cameraAngularSpeed = -0.05f * dt;
	cameraDelta.yaw = mouseDelta.x * cameraAngularSpeed;
	cameraDelta.pitch = mouseDelta.y * cameraAngularSpeed;
	if (IsKeyDown(KEY_W))
	{
		cameraDelta.forward = -cameraLinearSpeed;
	}
	if (IsKeyDown(KEY_S))
	{
		cameraDelta.forward = cameraLinearSpeed;
	}
	if (IsKeyDown(KEY_A))
	{
		cameraDelta.right = -cameraLinearSpeed;
	}
	if (IsKeyDown(KEY_D))
	{
		cameraDelta.right = cameraLinearSpeed;
	}
	if (IsKeyDown(KEY_LEFT_SHIFT))
	{
		cameraDelta.up = -cameraLinearSpeed;
	}
	if (IsKeyDown(KEY_SPACE))
	{
		cameraDelta.up = cameraLinearSpeed;
	}

	UpdateFpsCamera(camera, cameraDelta);
	return ToView(camera);
}
