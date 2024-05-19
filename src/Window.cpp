#include "Window.h"
#include "Config.h"
#include <iostream>
#include <array>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* userParam);
void OnResize(GLFWwindow* window, int width, int height);
void OnKeyInput(GLFWwindow* window, int key, int scancode, int action, int mods);

GLFWwindow* fWindow = nullptr;

struct Input
{
    std::array<int, KEY_COUNT> keysPrev{};
    std::array<int, KEY_COUNT> keysCurr{};
} fInput;

void CreateWindow()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    fWindow = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "S3D", NULL, NULL);
    if (fWindow == NULL)
    {
        glfwTerminate();
        return;
    }

    glfwSetWindowPos(fWindow, 100, 100);
    glfwMakeContextCurrent(fWindow);
    glfwSetFramebufferSizeCallback(fWindow, OnResize);
    glfwSetKeyCallback(fWindow, OnKeyInput);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        return;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(fWindow, true);
    ImGui_ImplOpenGL3_Init();
}

void DestroyWindow()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
}

bool IsKeyDown(int key)
{
    return fInput.keysCurr[key] == GLFW_PRESS;
}

bool IsKeyUp(int key)
{
    return fInput.keysCurr[key] == GLFW_RELEASE;
}

bool IsKeyPressed(int key)
{
    return
        fInput.keysPrev[key] == GLFW_PRESS &&
        fInput.keysCurr[key] == GLFW_RELEASE;
}

bool ShouldClose()
{
    return glfwWindowShouldClose(fWindow);
}

void Close()
{
    glfwSetWindowShouldClose(fWindow, GLFW_TRUE);
}

void Swap()
{
    glfwSwapBuffers(fWindow);
}

void Poll()
{
    memcpy(fInput.keysPrev.data(), fInput.keysCurr.data(), sizeof(int) * KEY_COUNT);
    glfwPollEvents();
}

void OnKeyInput(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // Discard GLFW_REPEAT so action is either GLFW_PRESS or GLFW_RELEASE.
    if (action == GLFW_REPEAT) return;
    fInput.keysCurr[key] = action;

    //const char* name = glfwGetKeyName(key, scancode);
    //const char* status = action == GLFW_PRESS ? "Down" : "Up";
    //printf("Key %s is %s\n", name, status);
}

void OnResize(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
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
