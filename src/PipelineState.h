#pragma once
#include <glad/glad.h>

struct PipelineState
{
	bool depthTest;
	bool depthWrite;
	bool faceCulling;
	bool wireframes;

	GLenum depthFunc;
	GLenum cullFace;
	GLenum windingOrder;
};

extern PipelineState gPipelineDefault;
extern PipelineState gPipelineWireframes;
extern PipelineState gPipelineNoDepth;

void InitPipelineState(); // Modifications from default gl state (ie enable depth test)
void QuitPipelineState(); // Destroys empty vao

PipelineState GetPipelineState();
void SetPipelineState(PipelineState state);

void BindEmptyVao();// vao with no buffers 
void BindNullVao(); // GL_NONE

// Faster to type if left as state machine,
// but more robust if we set all gl state before each draw call.

// Shader and Texture currently manage their own state. Might be better to move them here.
// At the same time, Shader & Texture have file-scope, so perhaps best left as is.
