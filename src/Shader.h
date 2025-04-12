#pragma once
#include <glad/glad.h>
#include <unordered_map>
#include "Math.h"

struct Shader
{
	GLuint id;
	std::unordered_map<const char*, GLint> locs;
};

extern Shader gShaderLine;
extern Shader gShaderFsq;
extern Shader gShaderFsqDepth;
extern Shader gShaderSkybox;
extern Shader gShaderEnvironmentReflect;
extern Shader gShaderEnvironmentRefract;

extern Shader gShaderColor;
extern Shader gShaderDepth;
extern Shader gShaderNormals;
extern Shader gShaderTcoords;
extern Shader gShaderTexture;
extern Shader gShaderPositionsWorld;
extern Shader gShaderPositionsScreen;

extern Shader gShaderDeferredGeometryBuffer;
extern Shader gShaderDeferredDirectionLight;
extern Shader gShaderDeferredLightVolumes;

extern Shader gShaderPlanetsRaster;
extern Shader gShaderPlanetsRaymarch;
extern Shader gShaderAsteroids;

extern Shader gShaderHexagonGridDistance;
//extern Shader gShaderHexagonGridRaster;
extern Shader gShaderGradientCubemap;

extern Shader gShaderRaymarchBase;
extern Shader gShaderFractal2D;
extern Shader gShaderFractal3D;
extern Shader gShaderFractalOrb;

extern Shader gShaderCars;

void CreateShaders();
void DestroyShaders();

void BindShader(Shader* shader);
void UnbindShader();

void SendInt(const char* name, int v);
void SendFloat(const char* name, float v);
void SendVec2(const char* name, const Vector2& v);
void SendVec3(const char* name, const Vector3& v);
void SendVec4(const char* name, const Vector4& v);
void SendMat3(const char* name, const Matrix& v);
void SendMat4(const char* name, const Matrix& v);

void SendIntArray(const char* name, int* v, int count);
void SendFloatArray(const char* name, float* v, int count);
void SendVec2Array(const char* name, Vector2* v, int count);
void SendVec3Array(const char* name, Vector3* v, int count);
void SendVec4Array(const char* name, Vector4* v, int count);
void SendMat3Array(const char* name, Matrix* v, int count);
void SendMat4Array(const char* name, Matrix* v, int count);
