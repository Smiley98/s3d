#include "Image.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void Flip(Image* image)
{
    stbi__vertical_flip(image->pixels.data(), image->width, image->height, sizeof(Color));
}

void LoadImage(Image* image, const char* path, bool flip)
{
    int width, height, channels;
    stbi_uc* pixels = stbi_load(path, &width, &height, &channels, 4);
    
    LoadImage(image, width, height);
    memcpy(image->pixels.data(), pixels, width * height * sizeof(Color));

    assert(pixels != nullptr && channels == 4);
    stbi_image_free(pixels);

    if (flip)
        Flip(image);
}

Image gImageDiffuse;

void CreateImages()
{
    LoadImage(&gImageDiffuse, "assets/textures/african_head_diffuse.png", true);
}

void DestroyImages()
{
    UnloadImage(&gImageDiffuse);
}
