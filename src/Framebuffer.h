#pragma once
#include "Texture.h"
#include <array>

struct Framebuffer
{
	GLuint id = GL_NONE;
	int width = -1;
	int height = -1;

	std::array<Texture, 8> colors{};
	size_t colorCount = 0;
	Texture depth{};
};

void CreateFramebuffer(Framebuffer* framebuffer, int width, int height);
void DestroyFramebuffer(Framebuffer* framebuffer);

// Filter nearest for exact operations like sampling g-buffer, filter linear for upscaling/downscaling ie blur
void AddColor(Framebuffer* framebuffer, int format, int type, int filter);
void AddDepth(Framebuffer* framebuffer);

void BindFramebuffer(Framebuffer framebuffer);
void UnbindFramebuffer();
