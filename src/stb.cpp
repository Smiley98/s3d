#define STB_IMAGE_IMPLEMENTATION
#include "stb.h"

void FlipVertically(void* pixels, int width, int height, int bytesPerPixel)
{
	stbi__vertical_flip(pixels, width, height, bytesPerPixel);
}
