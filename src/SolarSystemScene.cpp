#include "SolarSystemScene.h"
#include "Window.h"
#include "Time.h"
#include "Render.h"
#include "Camera.h"
#include "Framebuffer.h"

struct Planet
{
	Vector3 position;
	Vector3 color;

	float radius;
	float rotationSpeed;
	float orbitSpeed;
};

static const int PLANET_COUNT = 9;
Planet planets[PLANET_COUNT];
bool fRaymarch = false;
bool fDepth = false;
float fFov = PI * 0.5f;

float planetRadii[PLANET_COUNT];		// Raymarch only
Vector3 planetColors[PLANET_COUNT];		// Both
Matrix planetWorld[PLANET_COUNT];		// Rasterization world matrix
Matrix planetWorldInv[PLANET_COUNT];	// Raymarching world matrix
Matrix planetNormal[PLANET_COUNT];		// Raster only
Matrix planetMvp[PLANET_COUNT];			// Raster only

Cubemap fSkyboxSpace;
Framebuffer fFbo;

struct Asteroids
{
	GLuint vao = GL_NONE;
	GLuint pbo = GL_NONE;	// positions (1 asteroid)
	GLuint tbo = GL_NONE;	// tcoords (1 asteroid)
	GLuint mbo = GL_NONE;	// matrices (all asteroids
	int count = 0;
	int instances = 0;
} fAsteroids;

Texture fTexAsteroid;

void CreateAsteroidsInstance()
{
	Mesh asteroid;
	CreateMesh(&asteroid, "./assets/meshes/asteroid.obj", false);
	fAsteroids.count = asteroid.count;
	fAsteroids.instances = 250;

	std::vector<Matrix> transformations(fAsteroids.instances);
	for (int i = 0; i < transformations.size(); i++)
	{
		float angle = (float)i / (float)fAsteroids.instances * PI * 2.0f;
		float x = Random(50.0f, 100.0f);
		float z = Random(50.0f, 100.0f);

		Matrix translation = Translate(cosf(angle) * x, Random(-10.0f, 10.0f), sinf(angle) * z);
		Matrix rotation = Rotate(Normalize(Vector3{ Random(0.0f, 1.0f), Random(0.0f, 1.0f), Random(0.0f, 1.0f) }), Random(0.0f, PI));
		Matrix scale = Scale(Random(0.25f, 2.0f), Random(0.25f, 2.0f), Random(0.25f, 2.0f));
		transformations[i] = Transpose(scale * rotation * translation);
	}

	glGenVertexArrays(1, &fAsteroids.vao);
	glBindVertexArray(fAsteroids.vao);

	glGenBuffers(1, &fAsteroids.pbo);
	glBindBuffer(GL_ARRAY_BUFFER, fAsteroids.pbo);
	glBufferData(GL_ARRAY_BUFFER, asteroid.positions.size() * sizeof(Vector3), asteroid.positions.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), nullptr);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &fAsteroids.tbo);
	glBindBuffer(GL_ARRAY_BUFFER, fAsteroids.tbo);
	glBufferData(GL_ARRAY_BUFFER, asteroid.tcoords.size() * sizeof(Vector2), asteroid.tcoords.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vector2), nullptr);
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &fAsteroids.mbo);
	glBindBuffer(GL_ARRAY_BUFFER, fAsteroids.mbo);
	glBufferData(GL_ARRAY_BUFFER, transformations.size() * sizeof(Matrix), transformations.data(), GL_STATIC_DRAW);
	for (int i = 0; i < 4; i++)
	{
		int attribute = 2 + i;
		glVertexAttribPointer(attribute, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(Vector4), (void*)(i * sizeof(Vector4)));
		glEnableVertexAttribArray(attribute);
		glVertexAttribDivisor(attribute, 1);
	}

	glBindVertexArray(GL_NONE);
	DestroyMesh(&asteroid);
}

void SolarSystemScene::OnLoad()
{
	SetMousePosition({ SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f });
	SetMouseState(MOUSE_STATE_NORMAL);
	gCamera = FromView(LookAt({ 48.0f, 48.0f, 20.0f }, V3_ZERO, V3_UP));

	CreateTextureFromFile(&fTexAsteroid, "./assets/textures/asteroid.png");
	CreateAsteroidsInstance();

	const char* skyboxSpaceFiles[] =
	{
		"./assets/textures/space_x+.png",
		"./assets/textures/space_x-.png",
		"./assets/textures/space_y+.png",
		"./assets/textures/space_y-.png",
		"./assets/textures/space_z+.png",
		"./assets/textures/space_z-.png",
	};
	CreateCubemap(&fSkyboxSpace, skyboxSpaceFiles);

	// Note that depth must be attached in order for depth-test and depth-write to be performed
	// If you don't need to sample the depth-buffer, a renderbuffer can be attached for better performance.
	CreateFramebuffer(&fFbo, SCREEN_WIDTH, SCREEN_HEIGHT);
	AddColor(&fFbo, GL_RGBA, GL_UNSIGNED_BYTE, GL_NEAREST);
	AddDepth(&fFbo);

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
	DestroyFramebuffer(&fFbo);
	DestroyCubemap(&fSkyboxSpace);
}

void SolarSystemScene::OnUpdate(float dt)
{
	UpdateFpsCameraDefault(gCamera, dt);
	gView = ToView(gCamera);
	gProj = Perspective(fFov, SCREEN_ASPECT, 0.1f, 1000.0f);
	
	const float tt = TotalTime();
	const float rotSelfScale = 0.0004f * 1000.0f;
	const float rotOrbitYScale = 0.001f * 1000.0f;
	for (int i = 0; i < PLANET_COUNT; i++)
	{
		Planet& planet = planets[i];
		Matrix scale = Scale(V3_ONE * planet.radius);
		Matrix rotationSelf = RotateY(tt / planet.rotationSpeed * 0.5f);
		Matrix rotationOrbit = RotateY(tt / planet.orbitSpeed);
		rotationOrbit = planet.orbitSpeed > 0.0f ? rotationOrbit : MatrixIdentity();
		Matrix translation = Translate(planet.position);
		
		planetWorld[i] = scale * rotationSelf * translation * rotationOrbit;
		planetWorldInv[i] = Invert(rotationSelf * translation * rotationOrbit);
		planetNormal[i] = NormalMatrix(planetWorld[i]);
		planetMvp[i] = planetWorld[i] * gView * gProj;
	}

	if (IsKeyPressed(KEY_Q))
		fRaymarch = !fRaymarch;

	if (IsKeyPressed(KEY_E))
		fDepth = !fDepth;
}

void SolarSystemScene::OnDraw()
{
	float tt = TotalTime();

	BindFramebuffer(fFbo);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	BindShader(&gShaderAsteroids);
	Matrix mvp = gView * gProj;
	Matrix orbit = RotateY(5.0f * tt * DEG2RAD);
	SendMat4("u_mvp", mvp);
	SendMat4("u_orbit", orbit);
	BindTexture(fTexAsteroid);
	glBindVertexArray(fAsteroids.vao);
	glDrawArraysInstanced(GL_TRIANGLES, 0, fAsteroids.count, fAsteroids.instances);
	glBindVertexArray(GL_NONE);
	UnbindTexture(fTexAsteroid);
	UnbindShader();

	if (fRaymarch)
	{
		Vector2 resolution{ SCREEN_WIDTH, SCREEN_HEIGHT };
		Matrix cameraRotation = FpsRotation(gCamera);
		float near = gProj.m14 / (gProj.m10 - 1.0f);
		float far = gProj.m14 / (gProj.m10 + 1.0f);
		BindShader(&gShaderPlanetsRaymarch);
		
		// Raymarching data
		SendVec3("u_camPos", gCamera.position);
		SendMat3("u_camRot", cameraRotation);
		SendVec2("u_resolution", resolution);
		SendFloat("u_fov", tanf(fFov * 0.5f));
		SendFloat("u_near", near);
		SendFloat("u_far", far);

		// Planet data
		SendMat4Array("u_planetMatrices", planetWorldInv, PLANET_COUNT);
		SendVec3Array("u_planetColors", planetColors, PLANET_COUNT);
		SendFloatArray("u_planetRadii", planetRadii, PLANET_COUNT);
		SendVec3("u_sunPos", planets[0].position);

		BindEmptyVao();
		glDrawArrays(GL_TRIANGLES, 0, 3);
		BindNullVao();
		UnbindShader();
	}
	else
	{
		BindShader(&gShaderPlanetsRaster);
		SendMat4Array("u_mvp", planetMvp, PLANET_COUNT);
		SendMat4Array("u_world", planetWorld, PLANET_COUNT);
		SendMat3Array("u_normal", planetNormal, PLANET_COUNT);
		SendVec3Array("u_color", planetColors, PLANET_COUNT);
		SendVec3("u_sunPos", planets[0].position);
		DrawMeshInstanced(gMeshSphere, PLANET_COUNT);
		UnbindShader();
	}
	DrawSkybox(fSkyboxSpace);
	UnbindFramebuffer();

	if (fDepth)
		DrawDepth(fFbo);
	else
		DrawColor(fFbo, 0);
}
