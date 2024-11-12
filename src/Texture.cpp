#include "Texture.h"
#include "Image.h"
#include <stb_image.h>

inline GLuint GenTextureId()
{
    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
    return id;
}

void CreateTexture(Texture* texture, int width, int height)
{
    assert(texture->id == GL_NONE);

    texture->id = GenTextureId();
    texture->width = width;
    texture->height = height;

    BindTexture(*texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    UnbindTexture(*texture);
}

void DestroyTexture(Texture* texture)
{
    GLint current;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &current);
    assert(texture->id != current, "Can't delete a bound texture");
    assert(texture->id != GL_NONE, "Invalid texture handle");

    glDeleteTextures(1, &texture->id);
    texture->width = 0;
    texture->height = 0;
    texture->id = GL_NONE;
}

// Made this because texture-from-image is 1) slow and 2) forces 4 channels & nearest sampling
void CreateTextureFromFile(Texture* texture, const char* path, bool flip)
{
    int width, height, channels;
    stbi_uc* pixels = stbi_load(path, &width, &height, &channels, 0);
    assert(channels == 3 || channels == 4);
    GLenum format = channels == 3 ? GL_RGB : GL_RGBA;

    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, pixels);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
    stbi_image_free(pixels);

    texture->id = id;
    texture->width = width;
    texture->height = height;
}

void CreateTextureFromImage(Texture* texture, const Image& image)
{
    CreateTexture(texture, image.width, image.height);
    UpdateTexture(*texture, image);
}

void UpdateTexture(Texture texture, const Image& image)
{
    assert(!image.pixels.empty());
    assert(texture.id != GL_NONE);

    BindTexture(texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image.width, image.height, GL_RGBA, GL_UNSIGNED_BYTE, image.pixels.data());
    UnbindTexture(texture);
}

void CreateCubemap(Cubemap* cubemap, const char* path[6])
{
    glGenTextures(1, &cubemap->id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap->id);

    int width, height, channels;
    for (int i = 0; i < 6; i++)
    {
        stbi_uc* pixels = stbi_load(path[i], &width, &height, &channels, 0);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        stbi_image_free(pixels);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, GL_NONE);
}

void DestroyCubemap(Cubemap* cubemap)
{
    GLint current;
    glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, &current);
    assert(cubemap->id != current, "Can't delete a bound texture");
    assert(cubemap->id != GL_NONE, "Invalid texture handle");

    glDeleteTextures(1, &cubemap->id);
}
