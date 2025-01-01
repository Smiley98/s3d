#define STB_IMAGE_IMPLEMENTATION
#include "stb.h"

static bool fFlip = false;

bool GetVerticalFlip()
{
	return fFlip;
}

void SetVerticalFlip(bool flip)
{
	stbi_set_flip_vertically_on_load(flip);
	fFlip = flip;
}

void FlipVertically(void* pixels, int width, int height, int bytesPerPixel)
{
	stbi__vertical_flip(pixels, width, height, bytesPerPixel);
}
