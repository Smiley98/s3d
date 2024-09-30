#include "SolarSystemScene.h"
#include "Time.h"
#include "Render.h"
#include "Camera.h"
#include <array>

struct Planet
{
	Vector3 scale;
	Vector3 position;
	Vector3 color;

	float rotationSpeed;
	float orbitSpeed;

	Matrix world;
};

std::array<Planet, 4> planets;

void SolarSystemScene::OnLoad()
{
	gCamera.position = { 48.0f, 48.0f, 20.0f };

	planets[0].scale = V3_ONE * 10.0f;
	planets[0].position = V3_ZERO;
	planets[0].rotationSpeed = 24.0f;
	planets[0].orbitSpeed = 0.0f;
	planets[0].color = { 0.97f, 0.38f, 0.09f };

	planets[1].scale = V3_ONE;
	planets[1].position = V3_RIGHT * 10.0f;
	planets[1].rotationSpeed = 58.7f;
	planets[1].orbitSpeed = 0.5f;
	planets[1].color = { 0.45f, 0.07f, 0.006f };
	
	planets[2].scale = V3_ONE * 2.0f;
	planets[2].position = { 20.0f, 0.0f, 5.0f };
	planets[2].rotationSpeed = 243.0f;
	planets[2].orbitSpeed = 0.8f;
	planets[2].color = { 0.6f, 0.32f, 0.006f };
	
	planets[3].scale = V3_ONE * 4.0f;
	planets[3].position = V3_RIGHT * 10.0f;
	planets[3].rotationSpeed = 1.0f;
	planets[3].orbitSpeed = 1.0f;
	planets[3].color = { 0.07f, 0.028f, 0.61f };
}

void SolarSystemScene::OnUnload()
{
}

const float gRotSelfScale = 0.0004f * 1000.0f;
const float gRotOrbitYScale = 0.001f * 1000.0f;
const float gRotOrbitZScale = 0.00001f * 1000.0f;
void SolarSystemScene::OnUpdate(float dt)
{
	float tt = TotalTime();
	for (Planet& planet : planets)
	{
		Matrix s = Scale(planet.scale * 0.5f);
		Matrix rotSelf = RotateY(gRotSelfScale * tt / planet.rotationSpeed);
		Matrix rotOrbit = RotateY(gRotOrbitYScale * tt / planet.orbitSpeed);
		rotOrbit = planet.orbitSpeed > 0.0f ? rotOrbit : MatrixIdentity();
		Matrix t = Translate(planet.position);
		planet.world = s * rotSelf * t * rotOrbit;
	}

	gView = LookAt({ 48.0f, 48.0f, 20.0f }, V3_ZERO, V3_UP);
	gProj = Perspective(PI * 0.5f, SCREEN_ASPECT, 0.1f, 1000.0f);
	// scale * selfY * trans * orbitY
}

void SolarSystemScene::OnDraw()
{
	for (Planet& planet : planets)
	{
		Matrix mvp = planet.world * gView * gProj;
		Mesh sphere;
		GenSphere(sphere);
		DrawMeshFlat(sphere, mvp, planet.color);
		DestroyMesh(sphere);
	}
}

void SolarSystemScene::OnDrawImGui()
{
}
