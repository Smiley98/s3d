#pragma once
#include "Texture.h"

constexpr size_t MAX_ATTACHMENTS = 8;

struct Framebuffer
{
	GLuint id = GL_NONE;
	bool complete = false;
	int width = -1;
	int height = -1;

	size_t colorCount = 0;
	Texture2D colors[MAX_ATTACHMENTS]{};
	Texture2D depth{};
};

void CreateFramebuffer(Framebuffer* framebuffer, int width, int height);
void DestroyFramebuffer(Framebuffer* framebuffer);

// Filter nearest for exact operations like sampling g-buffer, filter linear for upscaling/downscaling ie blur
void AddColor(Framebuffer* framebuffer, int internalFormat, int format, int type, int filter);
void AddDepth(Framebuffer* framebuffer);
void CompleteFramebuffer(Framebuffer* framebuffer);

//void AddDepth(Framebuffer* framebuffer, int internalFormat, int format, int type, int filter);
//void AddStencil(Framebuffer* framebuffer, int internalFormat, int format, int type, int filter);

void BindFramebuffer(Framebuffer framebuffer);
void UnbindFramebuffer(Framebuffer framebuffer);
