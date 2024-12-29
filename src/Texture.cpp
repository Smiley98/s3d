#include "Texture.h"
#include "stb.h"

#include <cassert>
#include <unordered_map>

inline GLint QueryTexture()
{
    GLint id;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &id);
    return id;
}

inline GLint QueryCubemap()
{
    GLint id;
    glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, &id);
    return id;
}

// TODO -- Consider passing in type & internal-format parameters, or even removing this funciton entirely.
// The more abstractions, the more chances there are for texture-formatting errors to occur.
void CreateTextureFromFile(Texture* texture, const char* path, bool flip)
{
    int width, height, channels;
    stbi_uc* pixels = stbi_load(path, &width, &height, &channels, 0);

    // What if we have a greyscale texture??
    // Probably reasonable to keep this function and handle greyscale/outlier textures via CreateTextureFromMemory.
    assert(channels == 3 || channels == 4);
    GLenum format = channels == 3 ? GL_RGB : GL_RGBA;
    GLint internalFormat = channels == 3 ? GL_RGB8 : GL_RGBA8;
    if (flip)
        FlipVertically(pixels, width, height, channels);

    CreateTextureFromMemory(texture, width, height, internalFormat, format, GL_UNSIGNED_BYTE, GL_LINEAR, pixels);
    stbi_image_free(pixels);
}

void CreateTextureFromMemory(Texture* texture, int width, int height, int internalFormat, int format, int type, int filter, unsigned char* pixels)
{
    GLuint id = GL_NONE;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, pixels);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    texture->id = id;
    texture->width = width;
    texture->height = height;

    texture->internalFormat = internalFormat;
    texture->format = format;
    texture->type = type;
    texture->filter = filter;
}

void DestroyTexture(Texture* texture)
{
    assert(texture->id != QueryTexture(), "Can't delete a bound texture");
    assert(texture->id != GL_NONE, "Invalid texture handle");

    glDeleteTextures(1, &texture->id);

    texture->id = GL_NONE;
    texture->width = -1;
    texture->height = -1;
    texture->format = -1;
    texture->type = -1;
    texture->filter = -1;
}

void BindTexture(Texture texture, GLuint slot)
{
    glActiveTexture(GL_TEXTURE0 + slot);
    assert(texture.id != GL_NONE, "Can't bind null texture");
    assert(QueryTexture() == GL_NONE, "Must unbind texture");
    glBindTexture(GL_TEXTURE_2D, texture.id);
}

void UnbindTexture(Texture texture, GLuint slot)
{
    glActiveTexture(GL_TEXTURE0 + slot);
    assert(texture.id != GL_NONE, "Current texture is null");
    assert(QueryTexture() != GL_NONE, "No texture currently bound (nothing to unbind)");
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
}

void CreateCubemap(Cubemap* cubemap, const char* path[6])
{
    glGenTextures(1, &cubemap->id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap->id);

    int width, height, channels;
    for (int i = 0; i < 6; i++)
    {
        stbi_uc* pixels = stbi_load(path[i], &width, &height, &channels, 0);
        assert(channels == 3 || channels == 4);
        GLenum format = channels == 3 ? GL_RGB : GL_RGBA;
        GLint internalFormat = channels == 3 ? GL_RGB8 : GL_RGBA8;
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, pixels);
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
    assert(cubemap->id != QueryCubemap(), "Can't delete a bound cubemap");
    assert(cubemap->id != GL_NONE, "Invalid cubemap handle");

    glDeleteTextures(1, &cubemap->id);
    cubemap->id = GL_NONE;
}

void BindCubemap(Cubemap cubemap)
{
    assert(cubemap.id != GL_NONE, "Can't bind null cubemap");
    assert(cubemap.id != QueryCubemap(), "Cubemap already bound");
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap.id);
}

void UnbindCubemap(Cubemap cubemap)
{
    assert(cubemap.id != GL_NONE);
    assert(QueryCubemap() != GL_NONE, "No cubemap currently bound (nothing to unbind)");
    glBindTexture(GL_TEXTURE_CUBE_MAP, GL_NONE);
}

void CreateTextures()
{
}

void DestroyTextures()
{
}
