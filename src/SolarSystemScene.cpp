#include "SolarSystemScene.h"
#include "Window.h"
#include "Time.h"
#include "Render.h"
#include "Camera.h"
#include <array>

struct Planet
{
	//Vector3 scale;
	Vector3 position;
	Vector3 color;

	float radius;
	float rotationSpeed;
	float orbitSpeed;

	Matrix world;
};

std::array<Planet, 9> planets;
bool fRaymarch = false;
float fFov = PI * 0.5f;

// Raymarching uniforms
std::array<float, 9> planetRadii;
std::array<Vector3, 9> planetColors;
std::array<Matrix, 9> planetMatrices;

void SolarSystemScene::OnLoad()
{
	SetMousePosition({ SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f });
	SetMouseState(MOUSE_STATE_NORMAL);
	gCamera = FromView(LookAt({ 48.0f, 48.0f, 20.0f }, V3_ZERO, V3_UP));

	// Sun
	planets[0].radius = 5.0f;
	planets[0].rotationSpeed = 24.0f;
	planets[0].orbitSpeed = 0.0f;
	planets[0].position = V3_ZERO;
	planets[0].color = { 0.97f, 0.38f, 0.09f };

	// Mercury
	planets[1].radius = 0.5f;
	planets[1].rotationSpeed = 58.7f;
	planets[1].orbitSpeed = 0.5f;
	planets[1].position = V3_RIGHT * 10.0f;
	planets[1].color = { 0.45f, 0.07f, 0.006f };
	
	// Venus
	planets[2].radius = 1.0f;
	planets[2].rotationSpeed = 243.0f;
	planets[2].orbitSpeed = 0.8f;
	planets[2].position = V3_RIGHT * 20.0f;
	planets[2].color = { 0.6f, 0.32f, 0.006f };
	
	// Earth
	planets[3].radius = 2.0f;
	planets[3].rotationSpeed = 1.0f;
	planets[3].orbitSpeed = 1.0f;
	planets[3].position = V3_RIGHT * 30.0f;
	planets[3].color = { 0.07f, 0.028f, 0.61f };

	// Mars
	planets[4].radius = 1.5f;
	planets[4].rotationSpeed = 1.1f;
	planets[4].orbitSpeed = 2.0f;
	planets[4].position = V3_RIGHT * 40.0f;
	planets[4].color = { 0.79f, 0.07f, 0.006f };

	// Jupiter
	planets[5].radius = 4.0f;
	planets[5].rotationSpeed = 0.4f;
	planets[5].orbitSpeed = 11.0f;
	planets[5].position = V3_RIGHT * 50.0f;
	planets[5].color = { 0.32f, 0.13f, 0.13f };

	// Saturn
	planets[6].radius = 3.0f;
	planets[6].rotationSpeed = 0.5f;
	planets[6].orbitSpeed = 29.4f;
	planets[6].position = V3_RIGHT * 60.0f;
	planets[6].color = { 0.45f, 0.45f, 0.21f };

	// Uranus
	planets[7].radius = 3.5f;
	planets[7].rotationSpeed = 0.8f;
	planets[7].orbitSpeed = 84.07f;
	planets[7].position = V3_RIGHT * 70.0f;
	planets[7].color = { 0.13f, 0.13f, 0.32f };

	// Neptune
	planets[8].radius = 4.0f;
	planets[8].rotationSpeed = 0.9f;
	planets[8].orbitSpeed = 164.81f;
	planets[8].position = V3_RIGHT * 80.0f;
	planets[8].color = { 0.21f, 0.028f, 0.79f };

	for (int i = 0; i < 9; i++)
	{
		planetRadii[i] = planets[i].radius;
		planetColors[i] = planets[i].color;
	}
}

void SolarSystemScene::OnUnload()
{
}

void SolarSystemScene::OnUpdate(float dt)
{
	float tt = TotalTime();
	UpdateFpsCameraDefault(gCamera, dt);
	
	const float rotSelfScale = 0.0004f * 1000.0f;
	const float rotOrbitYScale = 0.001f * 1000.0f;
	for (int i = 0; i < 9; i++)
	{
		Planet& planet = planets[i];
		Matrix s = Scale(V3_ONE * planet.radius);
		Matrix rotSelf = RotateY(tt / planet.rotationSpeed * 0.5f);
		Matrix rotOrbit = RotateY(tt / planet.orbitSpeed);
		rotOrbit = planet.orbitSpeed > 0.0f ? rotOrbit : MatrixIdentity();
		Matrix t = Translate(planet.position);
		planet.world = s * rotSelf * t * rotOrbit;

		// Invert for raymarching since its in camera-space rather than view-space (scale send as radius)
		planetMatrices[i] = Invert(rotSelf * t * rotOrbit);
	}

	gView = ToView(gCamera);
	gProj = Perspective(fFov, SCREEN_ASPECT, 0.1f, 100.0f);

	if (IsKeyPressed(KEY_TAB))
		fRaymarch = !fRaymarch;
}

void SolarSystemScene::OnDraw()
{
	// TODO -- Make a scene for environment mapping
	// TODO -- Add transformation order to raymarching base so I remember why all this works xD
	// TODO -- Add lighting to raymarching and fix rasterized sun lighting
	DrawSkybox(gSkybox);

	if (fRaymarch)
	{
		float time = TotalTime();
		Vector2 resolution{ SCREEN_WIDTH, SCREEN_HEIGHT };
		Matrix rot = FpsRotation(gCamera);

		BindShader(&gShaderPlanetsRaymarch);
		SendFloatArray("u_planetRadii", planetRadii.data(), 9);
		SendVec3Array("u_planetColors", planetColors.data(), 9);
		SendMat4Array("u_planetMatrices", planetMatrices.data(), 9);

		SendMat3("u_camRot", rot);
		SendVec3("u_camPos", gCamera.position);
		SendFloat("u_fov", tanf(fFov * 0.5f));

		SendVec2("u_resolution", resolution);
		SendFloat("u_time", time);
		DrawFsq();
		UnbindShader();
	}
	else
	{
		BindShader(&gShaderPlanetsRaster);
		for (int i = 0; i < 9; i++)
		{
			const Planet& planet = planets[i];
			Matrix mvp = planet.world * gView * gProj;
			Matrix normal = NormalMatrix(planet.world);
			SendMat4("u_mvp", mvp);
			SendMat4("u_world", planet.world);
			SendMat3("u_normal", normal);
			SendVec3("u_camPos", gCamera.position);
			SendVec3("u_sunPos", planets[0].position);
			SendVec3("u_planetColor", planet.color);
			SendInt("u_planetIndex", i);
			DrawMesh(gMeshSphere);
		}
		UnbindShader();
	}
}

void SolarSystemScene::OnDrawImGui()
{
}
