#include "Framebuffer.h"
#include "Config.h"
#include <cassert>

static GLuint fBuffer = GL_NONE;

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
		DestroyTexture(&framebuffer->colors[i]);
	framebuffer->colorCount = 0;

	if (framebuffer->depth.id != GL_NONE)
		DestroyTexture(&framebuffer->depth);

	glDeleteFramebuffers(1, &framebuffer->id);
	framebuffer->id = GL_NONE;
	framebuffer->width = -1;
	framebuffer->height = -1;
}

void AddColor(Framebuffer* framebuffer, int format, int type, int filter)
{
	Texture& texture = framebuffer->colors[framebuffer->colorCount];
	CreateTextureFromMemoryEx(&texture, framebuffer->width, framebuffer->height, format, format, type, filter);

	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->id);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + framebuffer->colorCount, GL_TEXTURE_2D, texture.id, 0);

	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
	framebuffer->colorCount++;
}

void AddDepth(Framebuffer* framebuffer)
{
	Texture& texture = framebuffer->depth;
	//CreateTextureFromMemoryEx(&texture, framebuffer->width, framebuffer->height, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, GL_NEAREST);
	CreateTextureFromMemoryEx(&texture, framebuffer->width, framebuffer->height, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT, GL_NEAREST);

	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->id);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texture.id, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture.id, 0);

	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
}

void BindFramebuffer(Framebuffer framebuffer)
{
	assert(fBuffer != framebuffer.id, "Framebuffer already bound");
	fBuffer = framebuffer.id;
	glBindFramebuffer(GL_FRAMEBUFFER, fBuffer);
	glViewport(0, 0, framebuffer.width, framebuffer.height);
}

void UnbindFramebuffer()
{
	assert(fBuffer != 0, "Default framebuffer already bound");
	fBuffer = 0;
	glBindFramebuffer(GL_FRAMEBUFFER, fBuffer);
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}
