#include "Window.h"
#include <iostream>
#include <array>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "PipelineState.h"

void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* userParam);
void OnResize(GLFWwindow* window, int width, int height);
void OnKeyInput(GLFWwindow* window, int key, int scancode, int action, int mods);
void OnMouseInput(GLFWwindow* window, int button, int action, int mods);
void OnMouseMove(GLFWwindow* window, double xpos, double ypos);

GLFWwindow* fWindow = nullptr;

struct Input
{
    std::array<int, KEY_COUNT> keysPrev{};
    std::array<int, KEY_COUNT> keysCurr{};

    std::array<int, MOUSE_BUTTON_COUNT> mouseButtonsPrev{};
    std::array<int, MOUSE_BUTTON_COUNT> mouseButtonsCurr{};

    Vector2 mousePosPrev{};
    Vector2 mousePosCurr{};
    Vector2 mousePosDelta{};

    MouseState mouseState =  MOUSE_STATE_NORMAL;
} fInput;

void CreateWindow()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

#ifdef NDEBUG
#else
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    fWindow = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "S3D", NULL, NULL);
    if (fWindow == NULL)
    {
        glfwTerminate();
        return;
    }

    const GLFWvidmode* monitor = glfwGetVideoMode(glfwGetPrimaryMonitor());
    int x = monitor->width * 0.5f - SCREEN_WIDTH * 0.5f;
    int y = monitor->height * 0.5f - SCREEN_HEIGHT * 0.5f;
    glfwSetWindowPos(fWindow, x, y);
    glfwMakeContextCurrent(fWindow);
    glfwSetFramebufferSizeCallback(fWindow, OnResize);
    glfwSetKeyCallback(fWindow, OnKeyInput);
    glfwSetMouseButtonCallback(fWindow, OnMouseInput);
    glfwSetCursorPosCallback(fWindow, OnMouseMove);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        return;
    }

#ifdef NDEBUG
#else
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(glDebugOutput, nullptr);
#endif

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(fWindow, true);
    ImGui_ImplOpenGL3_Init();

    InitPipelineState();
}

void DestroyWindow()
{
    QuitPipelineState();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
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
    fInput.mousePosDelta = V2_ZERO;
    fInput.mousePosPrev = fInput.mousePosCurr;
    memcpy(fInput.mouseButtonsPrev.data(), fInput.mouseButtonsCurr.data(), sizeof(int) * MOUSE_BUTTON_COUNT);
    memcpy(fInput.keysPrev.data(), fInput.keysCurr.data(), sizeof(int) * KEY_COUNT);
    glfwPollEvents();
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

bool IsMouseDown(int button)
{
    return fInput.mouseButtonsCurr[button] == GLFW_PRESS;
}

bool IsMouseUp(int button)
{
    return fInput.mouseButtonsCurr[button] == GLFW_RELEASE;
}

bool IsMouseClicked(int button)
{
    return
        fInput.mouseButtonsPrev[button] == GLFW_PRESS &&
        fInput.mouseButtonsCurr[button] == GLFW_RELEASE;
}

Vector2 GetMousePosition()
{
    return fInput.mousePosCurr;
}

Vector2 GetMouseDelta()
{
    return fInput.mousePosDelta;
}

void SetMousePosition(Vector2 screen, bool updateDelta)
{
    glfwSetCursorPos(fWindow, screen.x, screen.y);
    if (updateDelta)
        fInput.mousePosPrev = fInput.mousePosCurr = screen;
}

void SetMouseState(MouseState state)
{
    int modes[]{ GLFW_CURSOR_NORMAL, GLFW_CURSOR_HIDDEN, GLFW_CURSOR_DISABLED };
    glfwSetInputMode(fWindow, GLFW_CURSOR, modes[state]);
    fInput.mouseState = state;
}

MouseState GetMouseState()
{
    return fInput.mouseState;
}

void OnKeyInput(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // Discard GLFW_REPEAT so action is either GLFW_PRESS or GLFW_RELEASE.
    if (action == GLFW_REPEAT) return;
    fInput.keysCurr[key] = action;

    // Log key name & status:
    //const char* name = glfwGetKeyName(key, scancode);
    //const char* status = action == GLFW_PRESS ? "Down" : "Up";
    //printf("Key %s is %s\n", name, status);
}

void OnMouseInput(GLFWwindow* window, int button, int action, int mods)
{
    fInput.mouseButtonsCurr[button] = action;
}

void OnMouseMove(GLFWwindow* window, double xpos, double ypos)
{
    fInput.mousePosCurr.x = xpos;
    fInput.mousePosCurr.y = ypos;
    fInput.mousePosDelta = fInput.mousePosCurr - fInput.mousePosPrev;
    //printf("%f %f\n", xpos, ypos);
    //printf("%f %f\n", fInput.mouseDelta.x, fInput.mouseDelta.y);
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
