#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

GLuint GenTexture()
{
    GLuint id;
    glGenTextures(1, &id);
    BindTexture(id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    return id;
}

GLuint LoadTexture(const char* path, bool flip)
{
    int width, height, channels;
    stbi_uc* pixels = stbi_load(path, &width, &height, &channels, 4);
    assert(pixels != nullptr && (channels == 3 || channels ==  4));
    if (flip)
        stbi__vertical_flip(pixels, width, height, 4);

    // "format" is the memory layout of the image (CPU)
    // "internal format" is the memory layout of the texture (CPU)
    // This makes it possible to convert between formats ie RGB to RGBA
    GLuint id = GenTexture();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, channels == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    
    UnbindTexture(id);
    return id;
}

GLuint LoadTexture(const Image& image)
{
    assert(!image.pixels.empty());

    GLuint id = GenTexture();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.pixels.data());
    
    UnbindTexture(id);
    return id;
}

void UnloadTexture(GLuint& id)
{
    GLint current;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &current);

    assert(id != current, "Can't delete a bound texture");
    glDeleteTextures(1, &id);
}

void UpdateTexture(GLuint id, const Image& image)
{
    assert(!image.pixels.empty());
    BindTexture(id);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image.width, image.height, GL_RGBA, GL_UNSIGNED_BYTE, image.pixels.data());
    UnbindTexture(id);
}

void LoadImage(Image& image, int width, int height)
{
    image.pixels.resize(width * height);
    image.width = width;
    image.height = height;
}

void UnloadImage(Image& image)
{
    image.pixels.clear();
    image.width = 0;
    image.height = 0;
}

void Flip(Image& image)
{
    stbi__vertical_flip(image.pixels.data(), image.width, image.height, sizeof(Color));
}

void Fill(Image& image, Color color)
{
    std::fill(image.pixels.begin(), image.pixels.end(), color);
}
