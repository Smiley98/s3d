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

Shader gShaderColor;
Shader gShaderDepth;
Shader gShaderNormals;
Shader gShaderTcoords;
Shader gShaderTexture;
Shader gShaderPositionsWorld;
Shader gShaderPositionsScreen;

Shader gShaderDeferredGeometryBuffer;
Shader gShaderDeferredDirectionLight;
Shader gShaderDeferredLightVolumes;

Shader gShaderPlanetsRaster;
Shader gShaderPlanetsRaymarch;
Shader gShaderAsteroids;

Shader gShaderHexagonGridDistance;
//Shader gShaderHexagonGridRaster;
Shader gShaderGradientCubemap;

Shader gShaderRaymarchBase;
Shader gShaderFractal2D;
Shader gShaderFractal3D;
Shader gShaderFractalOrb;

Shader gShaderCars;

GLuint CreateShader(GLint type, const char* path);
void CreateProgram(Shader* shader, GLuint vs, GLuint fs);
GLint GetUniform(const char* name);

void CreateShaders()
{
    // Vertex shaders
    GLuint vsPassThrough = CreateShader(GL_VERTEX_SHADER, "assets/shaders/pass_through.vert");
    GLuint vsFsq = CreateShader(GL_VERTEX_SHADER, "assets/shaders/fsq.vert");
    GLuint vsSkybox = CreateShader(GL_VERTEX_SHADER, "assets/shaders/skybox.vert");
    GLuint vsLine = CreateShader(GL_VERTEX_SHADER, "assets/shaders/line.vert");
    GLuint vsMvp = CreateShader(GL_VERTEX_SHADER, "assets/shaders/default.vert");
    GLuint vsPlanetsRaster = CreateShader(GL_VERTEX_SHADER, "assets/shaders/planets_raster.vert");
    GLuint vsAsteroids = CreateShader(GL_VERTEX_SHADER, "assets/shaders/asteroids.vert");

    // Generic fragment shaders
    GLuint fsSkybox = CreateShader(GL_FRAGMENT_SHADER, "assets/shaders/skybox.frag");
    GLuint fsEnvironmentReflect = CreateShader(GL_FRAGMENT_SHADER, "assets/shaders/environment_reflect.frag");
    GLuint fsEnvironmentRefract = CreateShader(GL_FRAGMENT_SHADER, "assets/shaders/environment_refract.frag");
    GLuint fsTextureDepth = CreateShader(GL_FRAGMENT_SHADER, "assets/shaders/texture_depth.frag");

    // Visualization fragment shaders
    GLuint fsColor = CreateShader(GL_FRAGMENT_SHADER, "assets/shaders/color.frag");
    GLuint fsDepth = CreateShader(GL_FRAGMENT_SHADER, "assets/shaders/depth.frag");
    GLuint fsNormals = CreateShader(GL_FRAGMENT_SHADER, "assets/shaders/normals.frag");
    GLuint fsTcoords = CreateShader(GL_FRAGMENT_SHADER, "assets/shaders/tcoords.frag");
    GLuint fsTexture = CreateShader(GL_FRAGMENT_SHADER, "assets/shaders/texture.frag");
    GLuint fsPositionsWorld = CreateShader(GL_FRAGMENT_SHADER, "assets/shaders/positions_world.frag");
    GLuint fsPositionsScreen = CreateShader(GL_FRAGMENT_SHADER, "assets/shaders/positions_screen.frag");

    // Deferred pipeline fragment shaders
    GLuint fsDeferredGeometryBuffer = CreateShader(GL_FRAGMENT_SHADER, "assets/shaders/deferred_geometry_buffer.frag");
    GLuint fsDeferredDirectionLight = CreateShader(GL_FRAGMENT_SHADER, "assets/shaders/deferred_direction_light.frag");
    GLuint fsDeferredLightVolumes = CreateShader(GL_FRAGMENT_SHADER, "assets/shaders/deferred_light_volumes.frag");

    // Solar system shaders
    GLuint fsPlanetsRaster = CreateShader(GL_FRAGMENT_SHADER, "assets/shaders/planets_raster.frag"); 
    GLuint fsPlanetsRaymarch = CreateShader(GL_FRAGMENT_SHADER, "assets/shaders/planets_raymarch.frag");
    GLuint fsAsteroids = CreateShader(GL_FRAGMENT_SHADER, "assets/shaders/asteroids.frag");

    // Hexagon grid shaders
    GLuint fsHexagonGridRaymarch = CreateShader(GL_FRAGMENT_SHADER, "assets/shaders/hexagon_grid_raymarch.frag");
    //GLuint fsHexagonGridRaster = CreateShader(GL_FRAGMENT_SHADER, "assets/shaders/hexagon_grid_raster.frag");
    GLuint fsGradientCubemap = CreateShader(GL_FRAGMENT_SHADER, "assets/shaders/gradient_cubemap.frag");

    // Raymarching & fractal shaders
    GLuint fsRaymarchBase = CreateShader(GL_FRAGMENT_SHADER, "assets/shaders/raymarch_base.frag");
    GLuint fsFractal2D = CreateShader(GL_FRAGMENT_SHADER, "assets/shaders/fractal_2D.frag");
    GLuint fsFractal3D = CreateShader(GL_FRAGMENT_SHADER, "assets/shaders/fractal_3D.frag");
    GLuint fsFractalOrb = CreateShader(GL_FRAGMENT_SHADER, "assets/shaders/fractal_orb.frag");

    GLuint fsCars = CreateShader(GL_FRAGMENT_SHADER, "assets/shaders/cars.frag");
    
    // Shader programs
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

    CreateProgram(&gShaderHexagonGridDistance, vsFsq, fsHexagonGridRaymarch);
    //CreateProgram(&gShaderHexagonGridRaster, vsHexagonGrid, fsHexagonGridRaster);
    CreateProgram(&gShaderGradientCubemap, vsFsq, fsGradientCubemap);

    CreateProgram(&gShaderColor, vsMvp, fsColor);
    CreateProgram(&gShaderDepth, vsMvp, fsDepth);
    CreateProgram(&gShaderNormals, vsMvp, fsNormals);
    CreateProgram(&gShaderTcoords, vsMvp, fsTcoords);
    CreateProgram(&gShaderTexture, vsMvp, fsTexture);
    CreateProgram(&gShaderPositionsWorld, vsMvp, fsPositionsWorld);
    CreateProgram(&gShaderPositionsScreen, vsMvp, fsPositionsScreen);

    CreateProgram(&gShaderDeferredGeometryBuffer, vsMvp, fsDeferredGeometryBuffer);
    CreateProgram(&gShaderDeferredDirectionLight, vsFsq, fsDeferredDirectionLight);
    CreateProgram(&gShaderDeferredLightVolumes, vsPassThrough, fsDeferredLightVolumes);

    CreateProgram(&gShaderPlanetsRaster, vsPlanetsRaster, fsPlanetsRaster);
    CreateProgram(&gShaderPlanetsRaymarch, vsFsq, fsPlanetsRaymarch);
    CreateProgram(&gShaderAsteroids, vsAsteroids, fsAsteroids);

    CreateProgram(&gShaderCars, vsMvp, fsCars);
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
    assert(fShader == nullptr);
    glUseProgram(shader->id);
    fShader = shader;
}

void UnbindShader()
{
    assert(fShader != nullptr);
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
    if (fShader->locs.find(name) != fShader->locs.end())
        return fShader->locs[name];

    GLint loc = glGetUniformLocation(fShader->id, name);
    assert(loc != -1);
    fShader->locs[name] = loc;
    return loc;
}

// Something like the following would be more efficient than above, but it would be a lot more work for minial gain:
//struct Shader2
//{
//    GLuint id;
//    GLint locs[16];
//};
//
//GLint GetLocation(Shader2* shader, const char* name)
//{
//    GLint loc = glGetUniformLocation(fShader->id, name);
//    return loc;
//}
//
//void CreateShaders()
//{
//    const char* LOC_NAME_MVP = "u_mvp";
//    const char* LOG_NAME_WORLD = "u_world";
//    const char* LOC_NAME_NORMAL = "u_normal";
//    const char* LOC_NAME_COLOR = "u_color";
//
//    int LOC_MVP = 0;
//    int LOC_WORLD = 1;
//    int LOC_NORMAL = 2;
//    int LOC_COLOR = 3;
//
//    Shader2 test;
//    test.locs[LOC_MVP] = GetLocation(&test, LOC_NAME_MVP);
//    test.locs[LOC_WORLD] = GetLocation(&test, LOG_NAME_WORLD);
//    test.locs[LOC_NORMAL] = GetLocation(&test, LOC_NAME_NORMAL);
//    test.locs[LOC_COLOR] = GetLocation(&test, LOC_NAME_COLOR);
//
//    float16 m;
//    glUniformMatrix4fv(test.locs[LOC_MVP], 1, GL_FALSE, m.v);
//}
//
// The above is only practical if shaders have standardized attribute locations and uniform locations.
// I'd need a system that uploads all uniforms with non-negative locations every draw call.
// It would only be advantageous if I have a genuine need to automate my draw-calls, which I do not.
// At the same time, there's nothing particularly different between my draw calls.
// Still, I don't think such a system would provide any notable benefits to s3d in its current state.
// Most functions are low-level wrappers around OpenGL. The programmer maintains OpenGL state manually.
// I plan on keeping things this way since p2 will not need many unique draw calls.
