#include "Framebuffer.h"
#include <cassert>

static GLuint fReadBuffer = GL_NONE;
static GLuint fDrawBuffer = GL_NONE;

void CreateFramebuffer(Framebuffer* framebuffer, int width, int height)
{
	assert(framebuffer->id == GL_NONE);
	glGenFramebuffers(1, &framebuffer->id);
	framebuffer->width = width;
	framebuffer->height = height;
}

void DestroyFramebuffer(Framebuffer* framebuffer)
{
	assert(framebuffer->id != GL_NONE);

	for (int i = 0; i < framebuffer->colorCount; i++)
	{
		DestroyTexture(&framebuffer->colors[i]);
		//glDeleteTextures(1, &framebuffer->colors[i]);
		//framebuffer->colors[i] = GL_NONE;
	}
	framebuffer->colorCount = 0;

	if (framebuffer->depth.id != GL_NONE)
		DestroyTexture(&framebuffer->depth);
	//if (framebuffer->depth != GL_NONE)
	//{
	//	glDeleteTextures(1, &framebuffer->depth);
	//	framebuffer->depth = GL_NONE;
	//}

	glDeleteFramebuffers(1, &framebuffer->id);
	framebuffer->id = GL_NONE;

	framebuffer->width = -1;
	framebuffer->height = -1;
}

void AddColor(Framebuffer* framebuffer, int format, int type, int filter)
{
	Texture& texture = framebuffer->colors[framebuffer->colorCount];
	CreateTextureFromMemoryEx(&texture, framebuffer->width, framebuffer->height, format, format, type, filter);
	framebuffer->colorCount++;

	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->id);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + framebuffer->colorCount, GL_TEXTURE_2D, texture.id, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
}

void AddDepth(Framebuffer* framebuffer)
{
	Texture& texture = framebuffer->depth;
	CreateTextureFromMemoryEx(&texture, framebuffer->width, framebuffer->height, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, GL_NEAREST);

	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->id);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texture.id, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
}

void BindReadBuffer(Framebuffer framebuffer)
{
	assert(fReadBuffer != framebuffer.id);
	fReadBuffer = framebuffer.id;
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fReadBuffer);
}

void BindDrawBuffer(Framebuffer framebuffer)
{
	assert(fDrawBuffer != framebuffer.id);
	fDrawBuffer = framebuffer.id;
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fDrawBuffer);
	glViewport(0, 0, framebuffer.width, framebuffer.height);
}

void BindDefaultReadBuffer()
{
	assert(fReadBuffer != 0);
	fReadBuffer = 0;
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fReadBuffer);
}

void BindDefaultDrawBuffer()
{
	assert(fDrawBuffer != 0);
	fDrawBuffer = 0;
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fDrawBuffer);
}
