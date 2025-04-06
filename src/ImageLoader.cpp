#include "ImageLoader.h"
#include <cassert>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// Channels vs Requested Channels is like Format vs Internal Format.
// Channels is how many channels the image has, ie 3-channel png image.
// Requested Channels is how many channels the image is loaded with, ie convert 3-channel to 4-channel.

void OnLoad(uint8_t* pixels, const char* file, int c, int rc);

uint8_t* LoadImage1D(const char* file, int* w, int* c, int rc)
{
	int h;
	uint8_t* pixels = stbi_load(file, w, &h, c, rc);
	OnLoad(pixels, file, *c, rc);
	return pixels;
}

uint8_t* LoadImage2D(const char* file, int* w, int* h, int* c, int rc, ImageFlip flip)
{
	uint8_t* pixels = stbi_load(file, w, h, c, rc);
	OnLoad(pixels, file, *c, rc);

	if (flip == FLIP_VERTICAL)
		FlipVertically(pixels, *w, *h, rc);

	return pixels;
}

uint8_t* LoadImage3D(const char* file, int* w, int* h, int* depth, int* c, int rc)
{
	// Wait till I actually need to load a LUT to handle this.
	return nullptr;
}

void UnloadImage(uint8_t* pixels)
{
	stbi_image_free(pixels);
}

void LoadImageCubemap(const char* name, const char* extension, int* w, int* h, int* c, int rc, uint8_t* pixels[6])
{
	assert(strlen(name) < 240);
	char axes[3] = { 'x', 'y', 'z' };
	char faces[2] = { '+', '-' };
	for (int i = 0; i < 6; i++)
	{
		char buffer[256];
		char axis = axes[i / 2]; // 0, 1, 2
		char face = faces[i % 2];// 0, 1
		sprintf(buffer, "%s_%c%c.%s", name, axis, face, extension);

		pixels[i] = stbi_load(buffer, w, h, c, rc);
		OnLoad(pixels[i], buffer, *c, rc);
	}
}

void UnloadImageCubemap(uint8_t* pixels[6])
{
	for (int i = 0; i < 6; i++)
		stbi_image_free(pixels[i]);
}

void FlipVertically(void* pixels, int width, int height, int bytesPerPixel)
{
	stbi__vertical_flip(pixels, width, height, bytesPerPixel);
}

void OnLoad(uint8_t* pixels, const char* file, int c, int rc)
{
	assert(pixels != nullptr);
	//assert(rc == 4, "Enforcing all images must have 4 channels currently");
	printf("Loaded image %s (src channels: %i, dst channels: %i)\n", file, c, rc);
}
