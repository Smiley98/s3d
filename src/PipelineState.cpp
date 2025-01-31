#include "PipelineState.h"
#include <cassert>

PipelineState gPipelineDefault;
PipelineState gPipelineWireframes;
PipelineState gPipelineFsq;

static PipelineState fState{};
static GLuint fVaoEmpty;

void SetDepthTest(bool depthTest);
void SetDepthWrite(bool depthWrite);
void SetFaceCulling(bool faceCulling);
void SetWireframes(bool wireframes);
void SetCullFace(GLenum cullFace);

void ValidatePipeline();

// *** Even if its a gl default, remember to set explicity so this tracks it ***
void InitPipelineState()
{
	SetDepthTest(true);
	SetDepthWrite(true);
	SetFaceCulling(true);
	SetWireframes(false);

	SetDepthFunc(GL_LEQUAL);
	SetWindingOrder(GL_CCW);

	ValidatePipeline();
	gPipelineDefault = gPipelineWireframes = gPipelineFsq = fState;

	gPipelineWireframes.wireframes = true;
	gPipelineWireframes.depthTest = false;
	gPipelineWireframes.depthWrite = false;

	gPipelineFsq.depthTest = false;
	gPipelineFsq.depthWrite = false;

	glGenVertexArrays(1, &fVaoEmpty);
}

void QuitPipelineState()
{
	glDeleteVertexArrays(1, &fVaoEmpty);
	fVaoEmpty = GL_NONE;
}

PipelineState GetPipelineState()
{
#if NDEBUG
	ValidatePipeline();
#endif
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
		SetFaceCulling(state.cullFace);

	if (state.windingOrder != fState.windingOrder)
		SetWindingOrder(state.windingOrder);
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
	glCullFace(cullFace);
	fState.cullFace = cullFace;
}

void SetWindingOrder(GLenum windingOrder)
{
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

/*
bool DepthTest()
{
#if NDEBUG
#else
	GLboolean depthTest = false;
	glGetBooleanv(GL_DEPTH_TEST, &depthTest);
	assert(depthTest == fState.depthTest);
#endif
	return fState.depthTest;
}

bool DepthWrite()
{
#if NDEBUG
#else
	GLboolean depthWrite = false;
	glGetBooleanv(GL_DEPTH_WRITEMASK, &depthWrite);
	assert(depthWrite == fState.depthWrite);
#endif
	return fState.depthWrite;
}

bool Wireframes()
{
#if NDEBUG
#else
	// Apparently a core context returns 1 value but compatability returns 2.
	// Not sure why my contexts seemingly differ between machines, but may as well just examine mode[0]
	int mode[2]{};
	int target = fState.wireframes ? GL_LINE : GL_FILL;
	glGetIntegerv(GL_POLYGON_MODE, mode);
	assert(mode[0] == target);
#endif
	return fState.wireframes;
}

GLenum FaceCulling()
{
	return fState.cullFace;
}

GLenum DepthFunc()
{
#if NDEBUG
#else
	GLint depthFunc = 0;
	glGetIntegerv(GL_DEPTH_FUNC, &depthFunc);
	assert(depthFunc == fState.depthFunc);
#endif
	return fState.depthFunc;
}

GLenum WindingOrder()
{
#if NDEBUG
#else
	GLint windingOrder = 0;
	glGetIntegerv(GL_FRONT_FACE, &windingOrder);
	assert(windingOrder == fState.windingOrder);
#endif
	return fState.windingOrder;
}
*/

void BindEmptyVao()
{
	glBindVertexArray(fVaoEmpty);
}

void BindNullVao()
{
	glBindVertexArray(GL_NONE);
}
