#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <Math.h>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "Mesh.h"
#include "Shader.h"

void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* userParam);
void OnResize(GLFWwindow* window, int width, int height);
void OnInput(GLFWwindow* window);
void OnGui();

const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

GLuint CreateTexture(const char* path)
{
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height, channels;
    unsigned char* data = stbi_load(path, &width, &height, &channels, 0);
    if (data)
    {
        assert(channels == 3 || channels == 4);
        GLenum format = channels == 3 ? GL_RGB : GL_RGBA;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    }
    else
    {
        assert(false, "Texture load failed");
    }
    stbi_image_free(data);
    return texture;
}

int main(const char* path)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, OnResize);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    GLuint texVan = CreateTexture("assets/textures/van.png");

    GLuint vsDefault = CreateShader(GL_VERTEX_SHADER, "assets/shaders/default.vert");
    GLuint vsGouraud = CreateShader(GL_VERTEX_SHADER, "assets/shaders/gouraud.vert");
    GLuint vsFsq = CreateShader(GL_VERTEX_SHADER, "assets/shaders/fsq.vert");

    GLuint fsColor = CreateShader(GL_FRAGMENT_SHADER, "assets/shaders/color.frag");
    GLuint fsGouraud = CreateShader(GL_FRAGMENT_SHADER, "assets/shaders/gouraud.frag");
    GLuint fsPhong = CreateShader(GL_FRAGMENT_SHADER, "assets/shaders/phong.frag");
    GLuint fsPhongMaps = CreateShader(GL_FRAGMENT_SHADER, "assets/shaders/phong_maps.frag");
    GLuint fsFractal = CreateShader(GL_FRAGMENT_SHADER, "assets/shaders/fractal.frag");
    //GLuint fsTexture = CreateShader(GL_FRAGMENT_SHADER, "assets/shaders/texture.frag");

    GLuint shaderColor = CreateProgram(vsDefault, fsColor);
    GLuint shaderPhong = CreateProgram(vsDefault, fsPhong);
    GLuint shaderPhongMaps = CreateProgram(vsDefault, fsPhongMaps);
    GLuint shaderGouraud = CreateProgram(vsGouraud, fsGouraud);
    //GLuint shaderFsq = CreateProgram(vsFsq, fsTexture);
    GLuint shaderFractal = CreateProgram(vsFsq, fsFractal);

    GLuint vaoFsq;
    glGenVertexArrays(1, &vaoFsq);

    float prev = glfwGetTime();
    float curr = prev;
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    while (!glfwWindowShouldClose(window))
    {
        // Input and deltas
        float dt = curr - prev, tt = glfwGetTime();
        prev = curr;
        curr = tt;
        OnInput(window);

        glClearColor(0.25f, 0.75f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
         
        glDepthMask(GL_FALSE);
        glUseProgram(shaderFractal);
        glUniform1f(glGetUniformLocation(shaderFractal, "u_time"), tt);
        glUniform2f(glGetUniformLocation(shaderFractal, "u_resolution"), SCR_WIDTH, SCR_HEIGHT);
        glBindVertexArray(vaoFsq);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glDepthMask(GL_TRUE);
        
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        OnGui();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}

void OnInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void OnResize(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void OnGui()
{

}

void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* userParam)
{
    // ignore non-significant error/warning codes
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

    std::cout << "---------------" << std::endl;
    std::cout << "Debug message (" << id << "): " << message << std::endl;

    switch (source)
    {
    case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
    case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
    case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
    } std::cout << std::endl;

    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
    case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
    case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
    case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
    case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
    case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
    case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
    } std::cout << std::endl;

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
    case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
    case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
    case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
    } std::cout << std::endl;
    std::cout << std::endl;
}