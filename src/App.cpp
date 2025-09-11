#include "App.h"
#include "Window.h"
#include "Scene.h"
#include "Render.h"
#include "Time.h"
#include "Timer.h"

#include <array>
#include <iostream>

#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#define LIMIT_FPS true
#define LOG_FPS true

// Best lock fps at 60 to avoid screen-tear & precision issues.
constexpr float UPDATE_FREQ = 1.0f / 50.0f;
constexpr double FRAME_TIME = 1.0f / 60.0f;
double fFrameDelta;

void Update(float dt);
void Draw();
void DrawGui();
void DrawImGui();

void Init()
{
	CreateWindow();
	CreateShaders();
	CreateMeshes();
	InitRenderer();
	InitSoftwareRenderer();
	Scene::Create(Scene::CPU_RASTERIZATION);
}

void Quit()
{
	Scene::Destroy();
	QuitSoftwareRenderer();
	QuitRenderer();
	DestroyMeshes();
	DestroyShaders();
	DestroyWindow();
}

void Loop()
{
	double frameStart;
	double frameEnd;
	double fps = 0.0;
	size_t frameCount = 0;
	std::array<double, 16> frameSamples;

#if !LIMIT_FPS
	Timer updateTimer;
	updateTimer.total = UPDATE_FREQ;
	updateTimer.current = UPDATE_FREQ + FLT_EPSILON;
	fFrameDelta = 0.0;
#else
	fFrameDelta = FRAME_TIME;
#endif

	while (!ShouldClose())
	{
		frameStart = glfwGetTime();
#if !LIMIT_FPS
		updateTimer.Tick(fFrameDelta);
		if (updateTimer.Expired())
		{
			updateTimer.Reset();
			Update(UPDATE_FREQ);
		}
#else
		Update(fFrameDelta);
#endif
		Draw();
		DrawGui();
		DrawImGui();

		Swap();
		frameEnd = glfwGetTime();
		fFrameDelta = frameEnd - frameStart;

#if LIMIT_FPS
		while (fFrameDelta < FRAME_TIME)
		{
			frameEnd = glfwGetTime();
			fFrameDelta = frameEnd - frameStart;
		}
#endif

		// Determine frame-rate (average across 16 frames)
		frameSamples[frameCount] = fFrameDelta;
		frameCount++;
		if (frameCount >= frameSamples.size())
		{
			frameCount = 0;
			fps = 0.0;
			for (size_t i = 0; i < frameSamples.size(); i++)
			{
				fps += frameSamples[i];
				frameSamples[i] = 0.0;
			}
			fps /= (double)frameSamples.size();
			fps = ceil(1.0 / fps);
		}

#if LOG_FPS
		static Timer logTimer;
		logTimer.total = 1.0f;
		logTimer.Tick(fFrameDelta);
		if (logTimer.Expired())
		{
			logTimer.Reset();
			printf("Fps: %f\n", fps);
		}
#endif

		// Poll events
		Poll();

		// Terminate program
		if (IsKeyDown(KEY_ESCAPE))
			Close();
	}
}

void Update(float dt)
{
	Scene::Update(dt);
}

void Draw()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Scene::Draw();
}

void DrawGui()
{
	Scene::DrawGui();
}

void DrawImGui()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	Scene::DrawImGui();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

double FrameTime()
{
	return fFrameDelta;
}

double TotalTime()
{
	return glfwGetTime();
}
