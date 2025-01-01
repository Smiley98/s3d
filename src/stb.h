#pragma once
#include <stb_image.h>

bool GetVerticalFlip();
void SetVerticalFlip(bool flip);

// Flip an image vertically
void FlipVertically(void* pixels, int width, int height, int bytesPerPixel);
