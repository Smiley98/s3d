#include "Camera.h"
#include "Window.h"
#include <array>

Camera gCamera;
Camera gCameraNdc;

Camera gCameraOrtho1;
Camera gCameraOrtho10;
Camera gCameraOrtho100;
Camera gCameraOrtho1000;

Camera gCameraPersp30;
Camera gCameraPersp60;
Camera gCameraPersp75;
Camera gCameraPersp90;

void CreateCameras()
{
	float left = -1.0f * SCREEN_ASPECT;
	float right = 1.0f * SCREEN_ASPECT;
	float top = 1.0f;
	float bot = -1.0f;

	std::array<float, 4> scales { 1.0f, 10.0f, 100.0f, 1000.0f };
	std::array<Camera*, 4> orthographic
	{
		&gCameraOrtho1,
		&gCameraOrtho10,
		&gCameraOrtho100,
		&gCameraOrtho1000
	};

	std::array<float, 4> fovs { 30.0f, 60.0f, 75.0f, 90.0f };
	std::array<Camera*, 4> perspective
	{
		&gCameraPersp30,
		&gCameraPersp60,
		&gCameraPersp75,
		&gCameraPersp90
	};

	for (size_t i = 0; i < orthographic.size(); i++)
	{
		float s = scales[i];
		Camera& cam = *orthographic[i];
		cam.position = V3_FORWARD * s;
		cam.proj = Ortho(left * s, right * s, bot * s, top * s, 0.01f, s);
		UpdateCamera(cam, {});
	}

	for (size_t i = 0; i < perspective.size(); i++)
	{
		float f = fovs[i] * DEG2RAD;
		Camera& cam = *perspective[i];
		cam.position = V3_FORWARD * 5.0f;
		cam.proj = Perspective(f, SCREEN_ASPECT, 0.01f, 1000.0f);
		UpdateCamera(cam, {});
	}

	// Identity camera by default. Set different cameras within scenes!
	gCamera = gCameraNdc;
}

void DestroyCameras()
{
	Camera c;
	gCamera = gCameraNdc =
		gCameraOrtho1 = gCameraOrtho10 = gCameraOrtho100 = gCameraOrtho1000 =
		gCameraPersp30 = gCameraPersp60 = gCameraPersp75 = gCameraPersp90 = c;
}

void UpdateCameraDefault(Camera& camera, float dt)
{
	if (IsKeyPressed(KEY_C))
		SetMouseState(GetMouseState() != MOUSE_STATE_NORMAL ? MOUSE_STATE_NORMAL : MOUSE_STATE_DISABLED);

	Vector2 mouseDelta = GetMouseState() != MOUSE_STATE_NORMAL ? GetMouseDelta() : V2_ZERO;
	CameraDelta cameraDelta;
	float cameraLinearSpeed = 10.0f * dt;
	float cameraAngularSpeed = 0.05f * dt;
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

	UpdateCamera(camera, cameraDelta);
}
