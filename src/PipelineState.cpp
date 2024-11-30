#include "PipelineState.h"
#include <cassert>

struct State
{
	bool depthTest;
	bool depthWrite;
	bool wireframes;
	GLenum depthFunc;
	GLint windingOrder;
} fState;

GLuint fVaoEmpty;

// *** Even if its a gl default, remember to set explicity so this tracks it ***
void InitPipelineState()
{
	SetDepthTest(true);
	SetDepthWrite(true);
	SetWireframes(false);
	fState.depthFunc = GL_LEQUAL;
	fState.windingOrder = GL_CCW;
	glDepthFunc(fState.depthFunc);
	glFrontFace(fState.windingOrder);

	assert(DepthTest() == true);
	assert(DepthWrite() == true);
	assert(Wireframes() == false);
	assert(DepthFunc() == GL_LEQUAL);
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

void SetDepthFunc(GLint depthFunc)
{
	fState.depthFunc = depthFunc;
	glDepthFunc(depthFunc);
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

GLint DepthFunc()
{
#if NDEBUG
#else
	GLint depthFunc = 0;
	glGetIntegerv(GL_DEPTH_FUNC, &depthFunc);
	assert(depthFunc == fState.depthFunc);
#endif
	return fState.depthFunc;
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
