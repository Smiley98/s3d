#pragma once
#include <glad/glad.h>

extern GLuint gShaderFSQ;
extern GLuint gShaderColor;

void CreateShaders();
void DestroyShaders();

void BindShader(GLuint shader);
void UnbindShader();