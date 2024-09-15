#include "PipelineState.h"
#include <cassert>

struct State
{
	bool depthTest;
	bool depthWrite;
	bool wireframes;
	GLint windingOrder;
} fState;

GLuint fVaoEmpty;

// *** Even if its a gl default, remember to set explicity so this tracks it ***
void InitPipelineState()
{
	SetDepthTest(true);
	SetDepthWrite(true);
	SetWireframes(false);
	SetWindingOrder(GL_CCW);

	assert(DepthTest() == true);
	assert(DepthWrite() == true);
	assert(Wireframes() == false);
	assert(WindingOrder() == GL_CCW);

	glGenVertexArrays(1, &fVaoEmpty);
}

void QuitPipelineState()
{
	glDeleteVertexArrays(1, &fVaoEmpty);
	fVaoEmpty = GL_NONE;
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

void SetWireframes(bool wireframes)
{
	fState.wireframes = wireframes;
	glPolygonMode(GL_FRONT_AND_BACK, wireframes ? GL_LINE : GL_FILL);
}

void SetWindingOrder(GLint windingOrder)
{
	fState.windingOrder = windingOrder;
	glFrontFace(windingOrder);
}

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
	// Not gonna validate this cause its obvious xD
	return fState.wireframes;
}

GLint WindingOrder()
{
#if NDEBUG
#else
	GLint windingOrder = 0;
	glGetIntegerv(GL_FRONT_FACE, &windingOrder);
	assert(windingOrder == fState.windingOrder);
#endif
	return fState.windingOrder;
}

void BindEmptyVao()
{
	glBindVertexArray(fVaoEmpty);
}

void BindNullVao()
{
	glBindVertexArray(GL_NONE);
}
