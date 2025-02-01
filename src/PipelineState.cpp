#include "PipelineState.h"
#include <cassert>

static PipelineState fState{};

PipelineState gPipelineDefault;
PipelineState gPipelineNoDepth;
PipelineState gPipelineWireframes;

void SetDepthTest(bool depthTest);
void SetDepthWrite(bool depthWrite);
void SetFaceCulling(bool faceCulling);
void SetWireframes(bool wireframes);

void SetCullFace(GLenum cullFace);
void SetDepthFunc(GLenum depthFunc);
void SetWindingOrder(GLenum windingOrder);

void ValidatePipeline();

void InitPipelineState()
{
	gPipelineDefault.depthTest = true;
	gPipelineDefault.depthWrite = true;
	gPipelineDefault.faceCulling = true;
	gPipelineDefault.wireframes = false;
	gPipelineDefault.depthFunc = GL_LEQUAL;
	gPipelineDefault.cullFace = GL_BACK;
	gPipelineDefault.windingOrder = GL_CCW;
	SetPipelineState(gPipelineDefault);

	gPipelineNoDepth = gPipelineDefault;
	gPipelineNoDepth.depthTest = false;
	gPipelineNoDepth.depthWrite = false;

	gPipelineWireframes = gPipelineDefault;
	gPipelineWireframes.wireframes = true;
	gPipelineWireframes.depthTest = false;
	gPipelineWireframes.depthWrite = false;
}

void QuitPipelineState()
{
	ValidatePipeline();
	SetPipelineState(gPipelineDefault);
}

PipelineState GetPipelineState()
{
	return fState;
}

void SetPipelineState(PipelineState state)
{
	if (state.depthTest != fState.depthTest)
		SetDepthTest(state.depthTest);

	if (state.depthWrite != fState.depthWrite)
		SetDepthWrite(state.depthWrite);

	if (state.wireframes != fState.wireframes)
		SetWireframes(state.wireframes);

	if (state.depthFunc != fState.depthFunc)
		SetDepthFunc(state.depthFunc);

	if (state.cullFace != fState.cullFace)
		SetCullFace(state.cullFace);

	if (state.windingOrder != fState.windingOrder)
		SetWindingOrder(state.windingOrder);

#if !NDEBUG
	ValidatePipeline();
#endif
}

void SetDepthTest(bool depthTest)
{
	fState.depthTest = depthTest;
	if (depthTest)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);
}

void SetDepthWrite(bool depthWrite)
{
	fState.depthWrite = depthWrite;
	glDepthMask(depthWrite);
}

void SetFaceCulling(bool faceCulling)
{
	fState.faceCulling = faceCulling;
	if (faceCulling)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);
}

void SetDepthFunc(GLenum depthFunc)
{
	fState.depthFunc = depthFunc;
	glDepthFunc(depthFunc);
}

void SetWireframes(bool wireframes)
{
	fState.wireframes = wireframes;
	glPolygonMode(GL_FRONT_AND_BACK, wireframes ? GL_LINE : GL_FILL);
}

void SetCullFace(GLenum cullFace)
{
	assert(cullFace == GL_FRONT || cullFace == GL_BACK || cullFace == GL_FRONT_AND_BACK);
	fState.cullFace = cullFace;
	glCullFace(cullFace);
}

void SetWindingOrder(GLenum windingOrder)
{
	assert(windingOrder == GL_CW || windingOrder == GL_CCW);
	fState.windingOrder = windingOrder;
	glFrontFace(windingOrder);
}

void ValidatePipeline()
{
	// Depth test enabled
	GLboolean depthTest = false;
	glGetBooleanv(GL_DEPTH_TEST, &depthTest);
	assert(depthTest == fState.depthTest);

	// Depth write enabled
	GLboolean depthWrite = false;
	glGetBooleanv(GL_DEPTH_WRITEMASK, &depthWrite);
	assert(depthWrite == fState.depthWrite);

	// Face culling enabled
	GLboolean faceCulling = false;
	glGetBooleanv(GL_CULL_FACE, &faceCulling);
	assert(faceCulling == fState.faceCulling);

	// Wireframes
	int mode[2]{};
	int target = fState.wireframes ? GL_LINE : GL_FILL;
	glGetIntegerv(GL_POLYGON_MODE, mode);
	assert(mode[0] == target);
	// (Apparently a core context returns 1 value but compatability returns 2).
	// (Not sure why my contexts seemingly differ between machines, but may as well just examine mode[0]).

	// Depth function (GL_LESS, GL_LEQUAL, etc)
	GLint depthFunc = 0;
	glGetIntegerv(GL_DEPTH_FUNC, &depthFunc);
	assert(depthFunc == fState.depthFunc);

	// Face culling (GL_FRONT vs GL_BACK vs GL_FRONT_AND_BACK)
	GLint cullFace = 0;
	glGetIntegerv(GL_CULL_FACE_MODE, &cullFace);
	assert(cullFace == fState.cullFace);

	// Winding order (GL_CW vs GL_CCW)
	GLint windingOrder = 0;
	glGetIntegerv(GL_FRONT_FACE, &windingOrder);
	assert(windingOrder == fState.windingOrder);
}
