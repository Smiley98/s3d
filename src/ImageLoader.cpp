#include "ImageLoader.h"
#include "stb.h"
#include <cassert>
#include <string>

uint8_t* LoadImage1D(const char* file, int* width, int* channels)
{
	int height; // 1
	return stbi_load(file, width, &height, channels, 0);
}

uint8_t* LoadImage2D(const char* file, int* width, int* height, int* channels, bool flip)
{
	SetVerticalFlip(flip);
	uint8_t* pixels = stbi_load(file, width, height, channels, 0);
	assert(pixels != nullptr);
	printf("Loaded image %s\n", file);
	SetVerticalFlip(false);
	return pixels;
}

uint8_t* LoadImage3D(const char* file, int* width, int* height, int* depth, int* channels)
{
	// Wait till I actually need to load a LUT to handle this.
	return nullptr;
}

void UnloadImage(uint8_t* pixels)
{
	stbi_image_free(pixels);
}

void LoadCubemap(const char* name, const char* extension, int* width, int* height, int* channels, uint8_t* pixels[6])
{
	assert(strlen(name) < 240);
	//SetVerticalFlip(flip);
	char axes[3] = { 'x', 'y', 'z' };
	char faces[2] = { '+', '-' };
	for (int i = 0; i < 6; i++)
	{
		char buffer[256];
		char axis = axes[i / 2]; // 0, 1, 2
		char face = faces[i % 2];// 0, 1
		sprintf(buffer, "%s_%c%c.%s", name, axis, face, extension);
		pixels[i] = stbi_load(buffer, width, height, channels, 0);
		assert(pixels[i] != nullptr);
	}
	printf("Loaded cubemap %s.%s\n", name, extension);
	//SetVerticalFlip(false);
}

void UnloadCubemap(uint8_t* pixels[6])
{
	for (int i = 0; i < 6; i++)
		stbi_image_free(pixels[i]);
}
