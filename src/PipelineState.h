#pragma once
#include <glad/glad.h>

struct PipelineState
{
	bool depthTest;
	bool depthWrite;
	bool wireframes;
	bool faceCulling;

	GLenum cullFace;
	GLenum depthFunc;
	GLenum windingOrder;
};

extern PipelineState gPipelineDefault;
extern PipelineState gPipelineWireframes;
extern PipelineState gPipelineNoDepth;

void InitPipelineState();
void QuitPipelineState();

PipelineState GetPipelineState();
void SetPipelineState(PipelineState state);
