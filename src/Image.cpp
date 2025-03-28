#include "Image.h"
#include "ImageLoader.h"
#include <cassert>

void CreateImageFromFile(Image* image, const char* path, bool flip)
{
    int width, height, channels;
    uint8_t* pixels = LoadImage2D(path, &width, &height, &channels, sizeof(Color), flip == true ? FLIP_VERTICAL : FLIP_NONE);

    CreateImageFromMemory(image, width, height, (Color*)pixels);
    UnloadImage(pixels);
}

void CreateImageFromMemory(Image* image, int width, int height, Color* pixels)
{
    image->pixels.resize(width * height);
    image->depth.resize(width * height);
    image->width = width;
    image->height = height;

    if (pixels != nullptr)
    {
        memcpy(image->pixels.data(), pixels, width * height * sizeof(Color));
    }
}

void DestroyImage(Image* image)
{
    image->pixels.resize(0);
    image->depth.resize(0);
    image->width = -1;
    image->height = -1;
}

void Flip(Image* image)
{
    FlipVertically(image->pixels.data(), image->width, image->height, sizeof(Color));
}
