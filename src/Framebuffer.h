#pragma once
#include "Texture.h"
#include <array>
#include <vector>

constexpr size_t MAX_ATTACHMENTS = 8;
using DrawBuffers = std::array<bool, MAX_ATTACHMENTS>;

struct Framebuffer
{
	GLuint id = GL_NONE;
	bool complete = false;

	int width = -1;
	int height = -1;

	std::array<Texture2D*, MAX_ATTACHMENTS> colors{};
	Texture2D* depth = nullptr;
};

void CreateFramebuffer(Framebuffer* framebuffer, int width, int height);
void DestroyFramebuffer(Framebuffer* framebuffer);

void CompleteFramebuffer(Framebuffer* framebuffer);

void BindFramebuffer(Framebuffer framebuffer, DrawBuffers* drawBuffers = nullptr);
void UnbindFramebuffer(Framebuffer framebuffer);

// Filter nearest for exact operations like sampling g-buffer, filter linear for upscaling/downscaling ie blur
//void CopyColor(Framebuffer src, Framebuffer dst, int srcIndex, int dstIndex, GLenum filter);
//void CopyDepth(Framebuffer src, Framebuffer dst);
