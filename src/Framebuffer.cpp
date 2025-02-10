#include "Framebuffer.h"
#include "Config.h"
#include <cassert>

static GLuint fBuffer = GL_NONE;
static void AttachTexture2D(GLuint framebuffer, GLuint texture, GLenum attachment);

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
	AttachTexture2D(framebuffer->id, texture.id, GL_COLOR_ATTACHMENT0 + framebuffer->colorCount);
	framebuffer->colorCount++;
}

void AddDepth(Framebuffer* framebuffer)
{
	CreateTexture2D(&framebuffer->depth, framebuffer->width, framebuffer->height, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT, GL_NEAREST);
	AttachTexture2D(framebuffer->id, framebuffer->depth.id, GL_DEPTH_ATTACHMENT);
}

void CompleteFramebuffer(Framebuffer* framebuffer)
{
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->id);
	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
	framebuffer->complete = true;
}

//void CopyColor(Framebuffer src, Framebuffer dst, int srcIndex, int dstIndex, GLenum filter)
//{
//	glBindFramebuffer(GL_READ_FRAMEBUFFER, src.id);
//	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst.id);
//	glReadBuffer(GL_COLOR_ATTACHMENT0 + srcIndex);
//	glDrawBuffer(GL_COLOR_ATTACHMENT0 + dstIndex);
//	glBlitFramebuffer(0, 0, src.width, src.height, 0, 0, dst.width, dst.height, GL_COLOR_BUFFER_BIT, filter);
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//	fBuffer = GL_NONE;
//}
//
//void CopyDepth(Framebuffer src, Framebuffer dst)
//{
//	glBindFramebuffer(GL_READ_FRAMEBUFFER, src.id);
//	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst.id);
//	glBlitFramebuffer(0, 0, src.width, src.height, 0, 0, dst.width, dst.height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//	fBuffer = GL_NONE;
//}

void BindFramebuffer(Framebuffer framebuffer, std::vector<GLenum> drawBuffers)
{
	assert(framebuffer.complete, "Forgot to call CompleteFramebufer");
	assert(fBuffer != framebuffer.id, "Framebuffer already bound");
	glViewport(0, 0, framebuffer.width, framebuffer.height);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.id);

	GLenum db[MAX_ATTACHMENTS];
	if (drawBuffers.empty())
	{
		for (int i = 0; i < framebuffer.colorCount; i++)
			db[i] = GL_COLOR_ATTACHMENT0 + i;
	}
	else
	{
		assert(drawBuffers.size() == framebuffer.colorCount);
		for (int i = 0; i < framebuffer.colorCount; i++)
			db[i] = drawBuffers[i];
	}
	glDrawBuffers(framebuffer.colorCount, db);
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

void AttachTexture2D(GLuint framebuffer, GLuint texture, GLenum attachment)
{
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
}

// Framebuffers are collections of pointers to textures on the GPU.
// An s3d Framebuffer *owns* multiple textures, so framebuffer create/destroy handles the lifetimes of associated textures.
// The advantage is that its simple.
// The disadvantage is its not as flexible as the raw GL implemetation; its no longer possible for a g-buffer & light-buffer to share the same depth-texture.
// For my simple rendering needs, I think its best to keep Framebuffer responsible for ownership and blit if I need information from another texture.
// 
// However, separating texture creation from framebuffer attachments would allow for:
//	-Custom depth textures
//  -No duplication between CreateTexture() and AddColor()
// I tried this just to see what it looks like. Could almost replace AddColor & AddDepth with a single AddTexture function.
// Both point to a texture2d. AddColor increments the colour-count, AddDepth uses DEPTH_ATTACHMENT instead of COLOR_ATTACHMENT.
// Would be kind of awkward to pass a GLenum and handle attachment-specific cases. Probably better to separate into color/depth/stencil functions.
// Only makes sense to do this if I *actually* need *that* much customization over framebuffers, and have a genuinely good reason to point to external textures.
// The above AttachTexture2D function is a proof of concept. Using it internally as a reminder that framebuffers DO NOT OWN texture memory on the GPU.
