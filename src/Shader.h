#pragma once
#include <glad/glad.h>
#include <unordered_map>
#include "Math.h"

struct Shader
{
	GLuint id;
	std::unordered_map<const char*, GLint> locs;
};

extern Shader gShaderFSQ;
extern Shader gShaderLine;
extern Shader gShaderColor;
extern Shader gShaderNormals;
extern Shader gShaderTcoords;
extern Shader gShaderPositionsWorld;
extern Shader gShaderPositionsScreen;

void CreateShaders();
void DestroyShaders();

void BindShader(Shader* shader);
void UnbindShader();

void SendFloat(const char* name, float v);
void SendInt(const char* name, int v);
void SendVec2(const char* name, const Vector2& v);
void SendVec3(const char* name, const Vector3& v);
void SendVec4(const char* name, const Vector4& v);
void SendMat3(const char* name, const Matrix* v);
void SendMat4(const char* name, const Matrix* v);
// Don't add quantity & transpose parameters unless they're actually needed.
