#pragma once
#include <glad/glad.h>

void InitPipelineState(); // Modifications from default gl state (ie enable depth test)
void QuitPipelineState(); // Destroys empty vao 

void SetDepthTest(bool depthTest);
void SetDepthWrite(bool depthWrite);
void SetWireframes(bool wireframes);
void SetWindingOrder(GLint windingOrder);

bool DepthTest();
bool DepthWrite();
bool Wireframes();
GLint WindingOrder();

void BindEmptyVao();// vao with no buffers 
void BindNullVao(); // GL_NONE

// Faster to type if left as state machine,
// but more robust if we set all gl state before each draw call.