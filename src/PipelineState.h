#pragma once
#include <glad/glad.h>

void InitPipelineState(); // Modifications from default gl state (ie enable depth test)
void QuitPipelineState(); // Destroys empty vao 

void SetDepthTest(bool depthTest);
void SetDepthWrite(bool depthWrite);
void SetWireframes(bool wireframes);
void SetFaceCulling(GLenum faceCulling);

bool DepthTest();
bool DepthWrite();
bool Wireframes();

GLenum FaceCulling();
GLint DepthFunc();
GLint WindingOrder();

void BindEmptyVao();// vao with no buffers 
void BindNullVao(); // GL_NONE

// Faster to type if left as state machine,
// but more robust if we set all gl state before each draw call.

// Shader and Texture currently manage their own state. Might be better to move them here.
// At the same time, Shader & Texture have file-scope, so perhaps best left as is.
