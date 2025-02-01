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

// TODO -- Make my own enums so I can't confuse it with OpenGL?
// (I already confused faceCulling with cullFace and that screwed my skyboxes without triggering assertions)...
// However, doing so is so verbose it almost defeats the purpose of my save/load architecture.
// Best solution is to be careful when implementing new draw calls & new pipeline states.
// As long as I test each implementation, silly errors like above won't happen (unless I do more large refactors).
