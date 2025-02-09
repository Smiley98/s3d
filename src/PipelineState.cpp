#include "PipelineState.h"
#include <cassert>

static PipelineState fPipelineInternal{};

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

void InitPipelineState()
{
	// Put gpu here to proove my gpu-get works since default depth is LESS but I set it to LEQUAl,etc.
	//PipelineState gpu = GetPipelineState();

	SetDepthTest(true);		// Default = false
	SetDepthWrite(true);	// Default = true
	SetWireframes(false);	// Default = false
	SetFaceCulling(true);	// Default = false
	SetCullFace(GL_BACK);	// Default = BACK
	SetDepthFunc(GL_LEQUAL);// Default = LESS
	SetWindingOrder(GL_CCW);// Default = CCW

	PipelineState cpu = fPipelineInternal;
	PipelineState gpu = GetPipelineState();

	assert(cpu.depthTest == gpu.depthTest);
	assert(cpu.depthWrite == gpu.depthWrite);
	assert(cpu.wireframes == gpu.wireframes);
	assert(cpu.faceCulling == gpu.faceCulling);

	assert(cpu.cullFace == gpu.cullFace);
	assert(cpu.depthFunc == gpu.depthFunc);
	assert(cpu.windingOrder == gpu.windingOrder);

	gPipelineDefault = cpu;

	gPipelineNoDepth = cpu;
	gPipelineNoDepth.depthTest = false;
	gPipelineNoDepth.depthWrite = false;

	gPipelineWireframes = cpu;
	gPipelineWireframes.wireframes = true;
	gPipelineWireframes.depthTest = false;
	gPipelineWireframes.depthWrite = false;
}

void QuitPipelineState()
{
}

PipelineState GetPipelineState()
{
	// Depth test enabled
	GLboolean depthTest = false;
	glGetBooleanv(GL_DEPTH_TEST, &depthTest);

	// Depth write enabled
	GLboolean depthWrite = false;
	glGetBooleanv(GL_DEPTH_WRITEMASK, &depthWrite);

	// Wireframes
	int mode[2]{};
	glGetIntegerv(GL_POLYGON_MODE, mode);
	// (Apparently a core context returns 1 value but compatability returns 2).
	// (Not sure why my contexts seemingly differ between machines, but may as well just examine mode[0]).

	// Face culling enabled
	GLboolean faceCulling = false;
	glGetBooleanv(GL_CULL_FACE, &faceCulling);

	// Face culling (GL_FRONT vs GL_BACK vs GL_FRONT_AND_BACK)
	GLint cullFace = 0;
	glGetIntegerv(GL_CULL_FACE_MODE, &cullFace);

	// Depth function (GL_LESS, GL_LEQUAL, etc)
	GLint depthFunc = 0;
	glGetIntegerv(GL_DEPTH_FUNC, &depthFunc);

	// Winding order (GL_CW vs GL_CCW)
	GLint windingOrder = 0;
	glGetIntegerv(GL_FRONT_FACE, &windingOrder);

	PipelineState gpu{};
	gpu.depthTest = depthTest;
	gpu.depthWrite = depthWrite;
	gpu.wireframes = mode[0] == GL_LINE;
	gpu.faceCulling = faceCulling;

	gpu.cullFace = cullFace;
	gpu.depthFunc = depthFunc;
	gpu.windingOrder = windingOrder;

	return gpu;
}

void SetPipelineState(PipelineState state)
{
	if (state.depthTest != fPipelineInternal.depthTest)
		SetDepthTest(state.depthTest);

	if (state.depthWrite != fPipelineInternal.depthWrite)
		SetDepthWrite(state.depthWrite);

	if (state.wireframes != fPipelineInternal.wireframes)
		SetWireframes(state.wireframes);

	if (state.faceCulling != fPipelineInternal.faceCulling)
		SetFaceCulling(state.faceCulling);

	if (state.cullFace != fPipelineInternal.cullFace)
		SetCullFace(state.cullFace);

	if (state.depthFunc != fPipelineInternal.depthFunc)
		SetDepthFunc(state.depthFunc);

	if (state.windingOrder != fPipelineInternal.windingOrder)
		SetWindingOrder(state.windingOrder);
}

void SetDepthTest(bool depthTest)
{
	fPipelineInternal.depthTest = depthTest;
	if (depthTest)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);
}

void SetDepthWrite(bool depthWrite)
{
	fPipelineInternal.depthWrite = depthWrite;
	glDepthMask(depthWrite);
}

void SetWireframes(bool wireframes)
{
	fPipelineInternal.wireframes = wireframes;
	glPolygonMode(GL_FRONT_AND_BACK, wireframes ? GL_LINE : GL_FILL);
}

void SetFaceCulling(bool faceCulling)
{
	fPipelineInternal.faceCulling = faceCulling;
	if (faceCulling)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);
}

void SetDepthFunc(GLenum depthFunc)
{
	fPipelineInternal.depthFunc = depthFunc;
	glDepthFunc(depthFunc);
}

void SetCullFace(GLenum cullFace)
{
	assert(cullFace == GL_FRONT || cullFace == GL_BACK || cullFace == GL_FRONT_AND_BACK);
	fPipelineInternal.cullFace = cullFace;
	glCullFace(cullFace);
}

void SetWindingOrder(GLenum windingOrder)
{
	assert(windingOrder == GL_CW || windingOrder == GL_CCW);
	fPipelineInternal.windingOrder = windingOrder;
	glFrontFace(windingOrder);
}
