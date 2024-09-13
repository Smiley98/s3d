#include "App.h"
#include "Window.h"
#include "Scene.h"
#include "Time.h"

#include "Shader.h"
#include "Mesh.h"
#include "Image.h"

#include <array>
#include <iostream>

#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

constexpr bool LOG_FPS = true;
constexpr double FRAME_TIME = 1.0f / 60.0f;
double fFrameDelta = FRAME_TIME;

void Update(float dt);
void Draw();
void DrawGui();
void DrawImGui();

void Init()
{
	CreateWindow();
	CreateShaders();
	CreateMeshes();
	CreateImages();

	Scene::Create(Scene::COLLISION);
}

void Loop()
{
	double logCurrent = 0.0;
	double logTotal = 1.0;

	double frameStart;
	double frameEnd;
	double fps = FRAME_TIME;
	size_t frameCount = 0;
	std::array<double, 16> frameSamples;
	while (!ShouldClose())
	{
		frameStart = glfwGetTime();
		Update(fFrameDelta);
		Draw();
		DrawGui();
		// Not sure if a game gui callback makes sense.
		// Is it better to render GUI at the end of Draw instead?

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		DrawImGui();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Swap back-buffer
		Swap();

		// Limit frame rate
		frameEnd = glfwGetTime();
		fFrameDelta = frameEnd - frameStart;
		while (fFrameDelta < FRAME_TIME)
		{
			frameEnd = glfwGetTime();
			fFrameDelta = frameEnd - frameStart;
		}

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

		// Log fps
		logCurrent += fFrameDelta;
		if (LOG_FPS && logCurrent >= logTotal)
		{
			logCurrent = 0.0;
			printf("Fps:  %f\n", fps);
		}

		// Poll events
		Poll();
		if (IsKeyDown(KEY_ESCAPE))
			Close();
	}
}

void Quit()
{
	Scene::Destroy();
	DestroyImages();
	DestroyMeshes();
	DestroyShaders();
	DestroyWindow();
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
	Scene::DrawImGui();
}

double FrameTime()
{
	return fFrameDelta;
}

double TotalTime()
{
	return glfwGetTime();
}
