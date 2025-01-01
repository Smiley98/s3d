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
		DestroyTexture2D(&framebuffer->colors[i]);
	framebuffer->colorCount = 0;

	if (framebuffer->depth.id != GL_NONE)
		DestroyTexture2D(&framebuffer->depth);

	glDeleteFramebuffers(1, &framebuffer->id);
	framebuffer->id = GL_NONE;
	framebuffer->complete = false;
	framebuffer->width = -1;
	framebuffer->height = -1;
}

void AddColor(Framebuffer* framebuffer, int internalFormat, int format, int type, int filter)
{
	Texture2D& texture = framebuffer->colors[framebuffer->colorCount];
	CreateTexture2D(&texture, framebuffer->width, framebuffer->height, internalFormat, format, type, filter);

	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->id);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + framebuffer->colorCount, GL_TEXTURE_2D, texture.id, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);

	framebuffer->colorCount++;
}

void AddDepth(Framebuffer* framebuffer)
{
	Texture2D& texture = framebuffer->depth;
	CreateTexture2D(&texture, framebuffer->width, framebuffer->height, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT, GL_NEAREST);

	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->id);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture.id, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);

	// TODO -- Test AddDepthStencil and AddStencil functions 
	//CreateTexture2D(&texture, framebuffer->width, framebuffer->height, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, GL_NEAREST);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texture.id, 0);
}

void CompleteFramebuffer(Framebuffer* framebuffer)
{
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->id);

	GLenum drawBuffers[MAX_ATTACHMENTS];
	for (size_t i = 0; i < framebuffer->colorCount; i++)
		drawBuffers[i] = GL_COLOR_ATTACHMENT0 + i;
	glDrawBuffers(framebuffer->colorCount, drawBuffers);

	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
	framebuffer->complete = true;
}

void BindFramebuffer(Framebuffer framebuffer)
{
	assert(framebuffer.complete, "Forgot to call CompleteFramebufer");
	assert(fBuffer != framebuffer.id, "Framebuffer already bound");
	glViewport(0, 0, framebuffer.width, framebuffer.height);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.id);
	fBuffer = framebuffer.id;
}

void UnbindFramebuffer(Framebuffer framebuffer)
{
	assert(fBuffer == framebuffer.id, "Framebuffer already unbound");
	assert(fBuffer != 0, "Default framebuffer already bound");
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
	fBuffer = GL_NONE;
}
