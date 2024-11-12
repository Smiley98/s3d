#include "SolarSystemScene.h"
#include "Window.h"
#include "Time.h"
#include "Render.h"
#include "Camera.h"
#include <array>
#include <stb_image.h>

struct Planet
{
	Vector3 scale;
	Vector3 position;
	Vector3 color;

	float rotationSpeed;
	float orbitSpeed;

	Matrix world;
};

std::array<Planet, 9> planets;

GLuint fCubemap;

void CreateCubemap(const char* path[6])
{
	glGenTextures(1, &fCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, fCubemap);

	int width, height, channels;
	for (int i = 0; i < 6; i++)
	{
		stbi_uc* pixels = stbi_load(path[i], &width, &height, &channels, 0);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
		stbi_image_free(pixels);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void SolarSystemScene::OnLoad()
{
	SetMousePosition({ SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f });
	SetMouseState(MOUSE_STATE_NORMAL);

	// Fps same as look-at -- success!!!
	//gCamera.position = { 48.0f, 48.0f, 20.0f };
	//gCamera.pitch = -42.0f * DEG2RAD;
	//gCamera.yaw = 67.0f * DEG2RAD;

	// Note that fps camera needs yaw then pitch, so whether they're 2 matrices or 2 eulers, they need to persist separately!
	gCamera = FromView(LookAt({ 48.0f, 48.0f, 20.0f }, V3_ZERO, V3_UP));
	float pitch = gCamera.pitch * RAD2DEG;
	float yaw = gCamera.yaw * RAD2DEG;

	const char* skyboxFiles[] =
	{
		"./assets/textures/sky_x+.png",
		"./assets/textures/sky_x-.png",
		"./assets/textures/sky_y+.png",
		"./assets/textures/sky_y-.png",
		"./assets/textures/sky_z+.png",
		"./assets/textures/sky_z-.png",
	};
	CreateCubemap(skyboxFiles);

	// TODO -- Make skybox shader.
	// Bind cube, only use position attributes.

	// Sun
	planets[0].scale = V3_ONE * 10.0f;
	planets[0].rotationSpeed = 24.0f;
	planets[0].orbitSpeed = 0.0f;
	planets[0].position = V3_ZERO;
	planets[0].color = { 0.97f, 0.38f, 0.09f };

	// Mercury
	planets[1].scale = V3_ONE;
	planets[1].rotationSpeed = 58.7f;
	planets[1].orbitSpeed = 0.5f;
	planets[1].position = V3_RIGHT * 10.0f;
	planets[1].color = { 0.45f, 0.07f, 0.006f };
	
	// Venus
	planets[2].scale = V3_ONE * 2.0f;
	planets[2].rotationSpeed = 243.0f;
	planets[2].orbitSpeed = 0.8f;
	planets[2].position = V3_RIGHT * 20.0f;
	planets[2].color = { 0.6f, 0.32f, 0.006f };
	
	// Earth
	planets[3].scale = V3_ONE * 4.0f;
	planets[3].rotationSpeed = 1.0f;
	planets[3].orbitSpeed = 1.0f;
	planets[3].position = V3_RIGHT * 30.0f;
	planets[3].color = { 0.07f, 0.028f, 0.61f };

	// Mars
	planets[4].scale = V3_ONE * 3.0f;
	planets[4].rotationSpeed = 1.1f;
	planets[4].orbitSpeed = 2.0f;
	planets[4].position = V3_RIGHT * 40.0f;
	planets[4].color = { 0.79f, 0.07f, 0.006f };

	// Jupiter
	planets[5].scale = V3_ONE * 8.0f;
	planets[5].rotationSpeed = 0.4f;
	planets[5].orbitSpeed = 11.0f;
	planets[5].position = V3_RIGHT * 50.0f;
	planets[5].color = { 0.32f, 0.13f, 0.13f };

	// Saturn
	planets[6].scale = V3_ONE * 6.0f;
	planets[6].rotationSpeed = 0.5f;
	planets[6].orbitSpeed = 29.4f;
	planets[6].position = V3_RIGHT * 60.0f;
	planets[6].color = { 0.45f, 0.45f, 0.21f };

	// Uranus
	planets[7].scale = V3_ONE * 7.0f;
	planets[7].rotationSpeed = 0.8f;
	planets[7].orbitSpeed = 84.07f;
	planets[7].position = V3_RIGHT * 70.0f;
	planets[7].color = { 0.13f, 0.13f, 0.32f };

	// Neptune
	planets[8].scale = V3_ONE * 8.0f;
	planets[8].rotationSpeed = 0.9f;
	planets[8].orbitSpeed = 164.81f;
	planets[8].position = V3_RIGHT * 80.0f;
	planets[8].color = { 0.21f, 0.028f, 0.79f };
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
	for (Planet& planet : planets)
	{
		Matrix s = Scale(planet.scale * 0.5f);
		Matrix rotSelf = RotateY(tt / planet.rotationSpeed * 0.5f);
		Matrix rotOrbit = RotateY(tt / planet.orbitSpeed);
		rotOrbit = planet.orbitSpeed > 0.0f ? rotOrbit : MatrixIdentity();
		Matrix t = Translate(planet.position);
		planet.world = s * rotSelf * t * rotOrbit;
	}

	gView = ToView(gCamera);
	gProj = Perspective(PI * 0.5f, SCREEN_ASPECT, 0.1f, 1000.0f);
}

void SolarSystemScene::OnDraw()
{
	Matrix mvpSkybox = NormalMatrix(gView) * gProj;
	glBindTexture(GL_TEXTURE_CUBE_MAP, fCubemap);
	BindShader(&gShaderSkybox);
	SendMat4("u_mvp", &mvpSkybox);
	SetDepthTest(false);
	DrawMesh(gMeshCube);
	SetDepthTest(true);
	UnbindShader();

	BindShader(&gShaderPlanetsRaster);
	for (Planet& planet : planets)
	{
		Matrix mvp = planet.world * gView * gProj;
		Matrix normal = NormalMatrix(planet.world);
		SendMat4("u_mvp", &mvp);
		SendMat4("u_world", &planet.world);
		SendMat3("u_normal", &normal);
		SendVec3("u_camPos", gCamera.position);
		SendVec3("u_sunPos", planets[0].position);
		SendVec3("u_planetColor", planet.color);
		DrawMesh(gMeshSphere);
	}
	UnbindShader();
}

void SolarSystemScene::OnDrawImGui()
{
}
