#include "App.h"
#include "Window.h"
#include "Scene.h"
#include "Shader.h"

#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

void Update();
void Draw();
void DrawImGui();

void Init()
{
	CreateWindow();
	CreateShaders();
	Scene::Create(Scene::TEST);
}

void Loop()
{
	// TODO -- time
	double prev = glfwGetTime();
	double curr = prev;
	while (!ShouldClose())
	{
		Update();
		Draw();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		DrawImGui();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		Swap();
		Poll();

		if (IsKeyDown(KEY_ESCAPE))
			Close();
	}
}

void Quit()
{
	Scene::Destroy();
	DestroyShaders();
	DestroyWindow();
}

float FrameTime()
{
	return 0.0f;
}

float TotalTime()
{
	return 0.0f;
}

void Update()
{
	Scene::Update(0.0f);
}

void Draw()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Scene::Draw();
}

void DrawImGui()
{
	Scene::DrawImGui();
}
