#include "Shader.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cassert>

std::vector<GLuint> fShaders;
std::vector<GLuint> fPrograms;

GLuint gShaderFSQ;
GLuint gShaderColor;

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

GLuint CreateProgram(GLuint vs, GLuint fs)
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
    return program;
}

void CreateShaders()
{
    GLuint vsFSQ = CreateShader(GL_VERTEX_SHADER, "assets/shaders/fsq.vert");
    GLuint fsTexture = CreateShader(GL_FRAGMENT_SHADER, "assets/shaders/texture.frag");

    GLuint vsMVP = CreateShader(GL_VERTEX_SHADER, "assets/shaders/default.vert");
    GLuint fsColor= CreateShader(GL_FRAGMENT_SHADER, "assets/shaders/color.frag");
    
    gShaderFSQ = CreateProgram(vsFSQ, fsTexture);
    gShaderColor = CreateProgram(vsMVP, fsColor);
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

void BindShader(GLuint shader)
{
    glUseProgram(shader);
}

void UnbindShader()
{
    glUseProgram(GL_NONE);
}
