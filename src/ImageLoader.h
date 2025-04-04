#pragma once
#include <cstdint>

enum ImageFlip
{
	FLIP_NONE,
	FLIP_VERTICAL
	//FLIP_HORIZONTAL
};

uint8_t* LoadImage1D(const char* file, int* w, int* c, int rc);
uint8_t* LoadImage2D(const char* file, int* w, int* h, int* c, int rc, ImageFlip flip);
uint8_t* LoadImage3D(const char* file, int* w, int* h, int* depth, int* c, int rc);
void UnloadImage(uint8_t* pixels);

void LoadImageCubemap(const char* name, const char* extension, int* w, int* h, int* c, int rc, uint8_t* pixels[6]);
void UnloadImageCubemap(uint8_t* pixels[6]);

void FlipVertically(void* pixels, int width, int height, int bytesPerPixel);
