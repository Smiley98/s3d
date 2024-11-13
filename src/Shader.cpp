#include "Shader.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cassert>

Shader* fShader = nullptr;
std::vector<GLuint> fShaders;
std::vector<GLuint> fPrograms;

Shader gShaderPassThrough;
Shader gShaderFSQ;
Shader gShaderSkybox;
Shader gShaderRaymarchBase;
Shader gShaderFractal2D;
Shader gShaderFractal3D;
Shader gShaderLine;
Shader gShaderColor;
Shader gShaderNormals;
Shader gShaderTcoords;
Shader gShaderTexture;
Shader gShaderPositionsWorld;
Shader gShaderPositionsScreen;
Shader gShaderPlanetsRaster;
Shader gShaderPlanetsRaymarch;

GLuint CreateShader(GLint type, const char* path);
void CreateProgram(Shader* shader, GLuint vs, GLuint fs);
GLint GetUniform(const char* name);

void CreateShaders()
{
    GLuint vsPassThrough = CreateShader(GL_VERTEX_SHADER, "assets/shaders/pass-through.vert");
    GLuint vsFSQ = CreateShader(GL_VERTEX_SHADER, "assets/shaders/fsq.vert");
    GLuint vsSkybox = CreateShader(GL_VERTEX_SHADER, "assets/shaders/skybox.vert");
    GLuint vsLine = CreateShader(GL_VERTEX_SHADER, "assets/shaders/line.vert");
    GLuint vsMVP = CreateShader(GL_VERTEX_SHADER, "assets/shaders/default.vert");
    GLuint vsPlanetsRaster = CreateShader(GL_VERTEX_SHADER, "assets/shaders/planets_raster.vert");

    GLuint fsPassThrough = CreateShader(GL_FRAGMENT_SHADER, "assets/shaders/pass-through.frag");
    GLuint fsSkybox = CreateShader(GL_FRAGMENT_SHADER, "assets/shaders/skybox.frag");
    GLuint fsRaymarchBase = CreateShader(GL_FRAGMENT_SHADER, "assets/shaders/raymarch_base.frag");
    GLuint fsFractal2D = CreateShader(GL_FRAGMENT_SHADER, "assets/shaders/fractal_2D.frag");
    GLuint fsFractal3D = CreateShader(GL_FRAGMENT_SHADER, "assets/shaders/fractal_3D.frag");
    GLuint fsColor = CreateShader(GL_FRAGMENT_SHADER, "assets/shaders/color.frag");
    GLuint fsNormals = CreateShader(GL_FRAGMENT_SHADER, "assets/shaders/normals.frag");
    GLuint fsTcoords = CreateShader(GL_FRAGMENT_SHADER, "assets/shaders/tcoords.frag");
    GLuint fsTexture = CreateShader(GL_FRAGMENT_SHADER, "assets/shaders/texture.frag");
    GLuint fsPositionsWorld = CreateShader(GL_FRAGMENT_SHADER, "assets/shaders/positions_world.frag");
    GLuint fsPositionsScreen = CreateShader(GL_FRAGMENT_SHADER, "assets/shaders/positions_screen.frag");
    GLuint fsPlanetsRaster = CreateShader(GL_FRAGMENT_SHADER, "assets/shaders/planets_raster.frag"); 
    GLuint fsPlanetsRaymarch = CreateShader(GL_FRAGMENT_SHADER, "assets/shaders/planets_raymarch.frag"); 
    
    CreateProgram(&gShaderPassThrough, vsPassThrough, fsPassThrough);
    CreateProgram(&gShaderFSQ, vsFSQ, fsTexture);
    CreateProgram(&gShaderSkybox, vsSkybox, fsSkybox);
    CreateProgram(&gShaderRaymarchBase, vsFSQ, fsRaymarchBase);
    CreateProgram(&gShaderFractal2D, vsFSQ, fsFractal2D);
    CreateProgram(&gShaderFractal3D, vsFSQ, fsFractal3D);
    CreateProgram(&gShaderLine, vsLine, fsColor);
    CreateProgram(&gShaderColor, vsMVP, fsColor);
    CreateProgram(&gShaderNormals, vsMVP, fsNormals);
    CreateProgram(&gShaderTcoords, vsMVP, fsTcoords);
    CreateProgram(&gShaderTexture, vsMVP, fsTexture);
    CreateProgram(&gShaderPositionsWorld, vsMVP, fsPositionsWorld);
    CreateProgram(&gShaderPositionsScreen, vsMVP, fsPositionsScreen);
    CreateProgram(&gShaderPlanetsRaster, vsPlanetsRaster, fsPlanetsRaster);
    CreateProgram(&gShaderPlanetsRaymarch, vsFSQ, fsPlanetsRaymarch);
}

void DestroyShaders()
{
    for (size_t i = 0; i < fPrograms.size(); i++)
        glDeleteProgram(fPrograms[i]);

    for (size_t i = 0; i < fShaders.size(); i++)
        glDeleteShader(fShaders[i]);

    fPrograms.clear();
    fShaders.clear();
}

void BindShader(Shader* shader)
{
    fShader = shader;
    glUseProgram(shader->id);
}

void UnbindShader()
{
    glUseProgram(GL_NONE);
    fShader = nullptr;
}

void SendFloat(const char* name, float v)
{
    glUniform1f(GetUniform(name), v);
}

void SendInt(const char* name, int v)
{
    glUniform1i(GetUniform(name), v);
}

void SendVec2(const char* name, const Vector2& v)
{
    glUniform2f(GetUniform(name), v.x, v.y);
}

void SendVec3(const char* name, const Vector3& v)
{
    glUniform3f(GetUniform(name), v.x, v.y, v.z);
}

void SendVec4(const char* name, const Vector4& v)
{
    glUniform4f(GetUniform(name), v.x, v.y, v.z, v.w);
}

void SendMat3(const char* name, const Matrix* v)
{
    float9 mat3 = ToFloat9(*v);
    glUniformMatrix3fv(GetUniform(name), 1, GL_FALSE, mat3.v);
}

void SendMat4(const char* name, const Matrix* v)
{
    float16 mat4 = ToFloat16(*v);
    glUniformMatrix4fv(GetUniform(name), 1, GL_FALSE, mat4.v);
}

GLuint CreateShader(GLint type, const char* path)
{
    GLuint shader = 0;
    try
    {
        // Load text file
        std::ifstream file;
        file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        file.open(path);

        // Interpret the file as a giant string
        std::stringstream stream;
        stream << file.rdbuf();
        file.close();

        // Verify shader type matches shader file extension
        const char* ext = strrchr(path, '.');
        switch (type)
        {
        case GL_VERTEX_SHADER:
            assert(strcmp(ext, ".vert") == 0);
            break;

        case GL_FRAGMENT_SHADER:
            assert(strcmp(ext, ".frag") == 0);
            break;
        default:
            assert(false, "Invalid shader type");
            break;
        }

        // Compile text as a shader
        std::string str = stream.str();
        const char* src = str.c_str();
        shader = glCreateShader(type);
        glShaderSource(shader, 1, &src, NULL);
        glCompileShader(shader);

        // Check for compilation errors
        GLint success;
        GLchar infoLog[512];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 512, NULL, infoLog);
            std::cout << "Shader failed to compile: \n" << infoLog << std::endl;
        }
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "Shader (" << path << ") not found: " << e.what() << std::endl;
    }

    fShaders.push_back(shader);
    return shader;
}

void CreateProgram(Shader* shader, GLuint vs, GLuint fs)
{
    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    // Check for linking errors
    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        program = GL_NONE;
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    fPrograms.push_back(program);
    shader->id = program;
}

GLint GetUniform(const char* name)
{
    if (fShader->locs.find(name) != fShader->locs.end())
        return fShader->locs[name];

    GLint loc = glGetUniformLocation(fShader->id, name);
    fShader->locs[name] = loc;
    return loc;
}
