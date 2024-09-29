#include "Camera.h"
#include "Window.h"

void UpdateCameraAuto(Camera& camera, float dt)
{
	CameraDelta cameraDelta;
	float cameraLinearSpeed = 10.0f * dt;
	float cameraAngularSpeed = 0.05f * dt;
	cameraDelta.yaw = MouseDelta().x * cameraAngularSpeed;
	cameraDelta.pitch = MouseDelta().y * cameraAngularSpeed;
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

	UpdateCamera(camera, cameraDelta);
}
