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
Shader gShaderLine;
Shader gShaderFsq;
Shader gShaderFsqDepth;
Shader gShaderSkybox;
Shader gShaderEnvironmentReflect;
Shader gShaderEnvironmentRefract;

Shader gShaderRaymarchBase;
Shader gShaderFractal2D;
Shader gShaderFractal3D;
Shader gShaderFractalOrb;

Shader gShaderColor;
Shader gShaderDepth;
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
    // Vertex shaders
    GLuint vsPassThrough = CreateShader(GL_VERTEX_SHADER, "assets/shaders/pass-through.vert");
    GLuint vsFsq = CreateShader(GL_VERTEX_SHADER, "assets/shaders/fsq.vert");
    GLuint vsSkybox = CreateShader(GL_VERTEX_SHADER, "assets/shaders/skybox.vert");
    GLuint vsLine = CreateShader(GL_VERTEX_SHADER, "assets/shaders/line.vert");
    GLuint vsMvp = CreateShader(GL_VERTEX_SHADER, "assets/shaders/default.vert");
    GLuint vsPlanetsRaster = CreateShader(GL_VERTEX_SHADER, "assets/shaders/planets_raster.vert");

    // Generic fragment shaders
    GLuint fsPassThrough = CreateShader(GL_FRAGMENT_SHADER, "assets/shaders/pass-through.frag");
    GLuint fsSkybox = CreateShader(GL_FRAGMENT_SHADER, "assets/shaders/skybox.frag");
    GLuint fsEnvironmentReflect = CreateShader(GL_FRAGMENT_SHADER, "assets/shaders/environment_reflect.frag");
    GLuint fsEnvironmentRefract = CreateShader(GL_FRAGMENT_SHADER, "assets/shaders/environment_refract.frag");
    GLuint fsTextureDepth = CreateShader(GL_FRAGMENT_SHADER, "assets/shaders/texture_depth.frag");

    // Raymarching fragment shaders
    GLuint fsRaymarchBase = CreateShader(GL_FRAGMENT_SHADER, "assets/shaders/raymarch_base.frag");
    GLuint fsFractal2D = CreateShader(GL_FRAGMENT_SHADER, "assets/shaders/fractal_2D.frag");
    GLuint fsFractal3D = CreateShader(GL_FRAGMENT_SHADER, "assets/shaders/fractal_3D.frag");
    GLuint fsFractalOrb = CreateShader(GL_FRAGMENT_SHADER, "assets/shaders/fractal_orb.frag");

    // Visualization fragment shaders
    GLuint fsColor = CreateShader(GL_FRAGMENT_SHADER, "assets/shaders/color.frag");
    GLuint fsDepth = CreateShader(GL_FRAGMENT_SHADER, "assets/shaders/depth.frag");
    GLuint fsNormals = CreateShader(GL_FRAGMENT_SHADER, "assets/shaders/normals.frag");
    GLuint fsTcoords = CreateShader(GL_FRAGMENT_SHADER, "assets/shaders/tcoords.frag");
    GLuint fsTexture = CreateShader(GL_FRAGMENT_SHADER, "assets/shaders/texture.frag");
    GLuint fsPositionsWorld = CreateShader(GL_FRAGMENT_SHADER, "assets/shaders/positions_world.frag");
    GLuint fsPositionsScreen = CreateShader(GL_FRAGMENT_SHADER, "assets/shaders/positions_screen.frag");

    // Solar system shaders
    GLuint fsPlanetsRaster = CreateShader(GL_FRAGMENT_SHADER, "assets/shaders/planets_raster.frag"); 
    GLuint fsPlanetsRaymarch = CreateShader(GL_FRAGMENT_SHADER, "assets/shaders/planets_raymarch.frag");
    
    // Shader programs
    CreateProgram(&gShaderPassThrough, vsPassThrough, fsPassThrough);
    CreateProgram(&gShaderFsq, vsFsq, fsTexture);
    CreateProgram(&gShaderFsqDepth, vsFsq, fsTextureDepth);
    CreateProgram(&gShaderSkybox, vsSkybox, fsSkybox);
    CreateProgram(&gShaderEnvironmentReflect, vsMvp, fsEnvironmentReflect);
    CreateProgram(&gShaderEnvironmentRefract, vsMvp, fsEnvironmentRefract);
    CreateProgram(&gShaderLine, vsLine, fsColor);

    CreateProgram(&gShaderRaymarchBase, vsFsq, fsRaymarchBase);
    CreateProgram(&gShaderFractal2D, vsFsq, fsFractal2D);
    CreateProgram(&gShaderFractal3D, vsFsq, fsFractal3D);
    CreateProgram(&gShaderFractalOrb, vsFsq, fsFractalOrb);

    CreateProgram(&gShaderColor, vsMvp, fsColor);
    CreateProgram(&gShaderDepth, vsMvp, fsDepth);
    CreateProgram(&gShaderNormals, vsMvp, fsNormals);
    CreateProgram(&gShaderTcoords, vsMvp, fsTcoords);
    CreateProgram(&gShaderTexture, vsMvp, fsTexture);
    CreateProgram(&gShaderPositionsWorld, vsMvp, fsPositionsWorld);
    CreateProgram(&gShaderPositionsScreen, vsMvp, fsPositionsScreen);

    CreateProgram(&gShaderPlanetsRaster, vsPlanetsRaster, fsPlanetsRaster);
    CreateProgram(&gShaderPlanetsRaymarch, vsFsq, fsPlanetsRaymarch);
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

void SendInt(const char* name, int v)
{
    glUniform1i(GetUniform(name), v);
}

void SendFloat(const char* name, float v)
{
    glUniform1f(GetUniform(name), v);
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

void SendMat3(const char* name, const Matrix& v)
{
    float9 mat3 = ToFloat9(v);
    glUniformMatrix3fv(GetUniform(name), 1, GL_FALSE, mat3.v);
}

void SendMat4(const char* name, const Matrix& v)
{
    float16 mat4 = ToFloat16(v);
    glUniformMatrix4fv(GetUniform(name), 1, GL_FALSE, mat4.v);
}

void SendIntArray(const char* name, int* v, int count)
{
    glUniform1iv(GetUniform(name), count, v);
}

void SendFloatArray(const char* name, float* v, int count)
{
    glUniform1fv(GetUniform(name), count, v);
}

void SendVec2Array(const char* name, Vector2* v, int count)
{
    glUniform2fv(GetUniform(name), count, (float*)v);
}

void SendVec3Array(const char* name, Vector3* v, int count)
{
    glUniform3fv(GetUniform(name), count, (float*)v);
}

void SendVec4Array(const char* name, Vector4* v, int count)
{
    glUniform4fv(GetUniform(name), count, (float*)v);
}

void SendMat3Array(const char* name, Matrix* v, int count)
{
    float9* mat3 = new float9[count];
    for (int i = 0; i < count; i++)
        mat3[i] = ToFloat9(v[i]);
    glUniformMatrix3fv(GetUniform(name), count, GL_FALSE, (float*)mat3);
    delete[] mat3;
}

void SendMat4Array(const char* name, Matrix* v, int count)
{
    float16* mat4 = new float16[count];
    for (int i = 0; i < count; i++)
        mat4[i] = ToFloat16(v[i]);
    glUniformMatrix4fv(GetUniform(name), count, GL_FALSE, (float*)mat4);
    delete[] mat4;
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
    // TODO -- consider shader reflection.
    // Instead of guess-and-check, compile shaders and map their uniforms once on-compile rather than on-draw.
    if (fShader->locs.find(name) != fShader->locs.end())
        return fShader->locs[name];

    // I don't think this has consequences, but kind of jank because if the attribute is not found then locs[name] = -1
    GLint loc = glGetUniformLocation(fShader->id, name);
    fShader->locs[name] = loc;
    return loc;
}
