#include "Image.h"
#include "stb.h"
#include <cassert>

void CreateImageDefault(Image* image)
{
    CreateImageFromMemory(image, CPU_IMAGE_SIZE, CPU_IMAGE_SIZE);
}

void CreateImageFromFile(Image* image, const char* path, bool flip)
{
    // Color is RGBA so channels must be 4
    int width, height, channels;
    stbi_uc* pixels = stbi_load(path, &width, &height, &channels, 4);
    assert(pixels != nullptr && channels == 4);
    CreateImageFromMemory(image, width, height, (Color*)pixels);
    stbi_image_free(pixels);

    if (flip)
        Flip(image);
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
