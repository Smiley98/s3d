#include "App.h"
#include "Window.h"

// Temporary till I can compile and switch to scenes xD
#include "Shader.h"
#include "Texture.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

GLuint texture;
GLuint vsFsq;
GLuint fsTexture;
GLuint shaderFsq;
GLuint vaoFsq;

void Update();
void Draw();
void DrawImGui();

void Init()
{
	InitWindow();

	Image image;
	LoadImage(image, 64, 64);
	for (int i = 0; i < image.height; i++)
	{
		for (int j = 0; j < image.width; j++)
		{
			Color2 uv = { j / (float)image.width, i / (float)image.height };
			uv.r = uv.r * 2.0f - 1.0f;
			uv.g = uv.g * 2.0f - 1.0f;

			Color color = Convert(uv);
			color.a = 255;

			//Color colors[] = { RED, GREEN, BLUE };
			image.pixels[i * image.width + j] = color;
		}
	}
	Flip(image);
	texture = LoadTexture(image);
	UnloadImage(image);
	glGenVertexArrays(1, &vaoFsq);

	vsFsq = CreateShader(GL_VERTEX_SHADER, "assets/shaders/fsq.vert");
	fsTexture = CreateShader(GL_FRAGMENT_SHADER, "assets/shaders/texture.frag");
	shaderFsq = CreateProgram(vsFsq, fsTexture);
}

void Loop()
{
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
	
	UnloadTexture(texture);
	QuitWindow();
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

}

void Draw()
{
	glClearColor(0.25f, 0.75f, 0.9f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	BindTexture(texture);
	glDepthMask(GL_FALSE);
	glUseProgram(shaderFsq);
	glUniform1i(glGetUniformLocation(shaderFsq, "u_tex"), 0);
	glBindVertexArray(vaoFsq);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	glBindVertexArray(GL_NONE);
	glUseProgram(GL_NONE);
	glDepthMask(GL_TRUE);
	UnbindTexture(texture);
}

void DrawImGui()
{

}
