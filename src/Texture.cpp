#include "Texture.h"
#include "stb.h"

#include <cassert>
#include <unordered_map>

static std::unordered_map<GLuint, char[64]> fTextureNames;
static GLuint fTexture = GL_NONE;
static GLuint fCubemap = GL_NONE;

inline void AddName(GLuint id, const char* name = nullptr)
{
#if _DEBUG
    assert(fTextureNames.find(id) == fTextureNames.end());
    if (name != nullptr)
        strcpy(fTextureNames[id], name);
    else
        sprintf(fTextureNames[id], "Unnamed texture %i", id);
#endif
}

inline void RemoveName(GLuint id)
{
#if _DEBUG
    assert(fTextureNames.find(id) != fTextureNames.end());
    fTextureNames.erase(id);
#endif
}

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

// Made this because texture-from-image is 1) slow and 2) forces 4 channels & nearest sampling
void CreateTextureFromFile(Texture* texture, const char* path, bool flip)
{
    int width, height, channels;
    stbi_uc* pixels = stbi_load(path, &width, &height, &channels, 0);

    assert(channels == 3 || channels == 4);
    GLenum format = channels == 3 ? GL_RGB : GL_RGBA;
    GLint internalFormat = channels == 3 ? GL_RGB8 : GL_RGBA8;
    if (flip)
        FlipVertically(pixels, width, height, channels);

    CreateTextureFromMemoryEx(texture, width, height, internalFormat, format, GL_UNSIGNED_BYTE, GL_LINEAR, pixels);
    stbi_image_free(pixels);
}

void CreateTextureFromMemory(Texture* texture, int width, int height, unsigned char* pixels/*RGBA -- 4 channels * 8 bits per channel assumed*/)
{
    CreateTextureFromMemoryEx(texture, width, height, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, GL_NEAREST, pixels);
}

void CreateTextureFromMemoryEx(Texture* texture, int width, int height, int internalFormat, int format, int type, int filter, unsigned char* pixels)
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
    texture->format = format;
    texture->type = type;
    texture->filter = filter;
    AddName(id);
}

void UpdateTexture(Texture texture, unsigned char* pixels)
{
    assert(pixels != nullptr);
    BindTexture(texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, texture.width, texture.height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    UnbindTexture(texture);
}

void DestroyTexture(Texture* texture)
{
    assert(texture->id != QueryTexture(), "Can't delete a bound texture");
    assert(texture->id != GL_NONE, "Invalid texture handle");

    RemoveName(texture->id);
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
    assert(texture.id != GL_NONE, "Can't bind null texture");
    assert(texture.id != QueryTexture(), "Texture already bound");
    
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, texture.id);
    fTexture = texture.id;
}

void UnbindTexture(Texture texture)
{
    assert(texture.id != GL_NONE, "Current texture is null");
    assert(QueryTexture() != GL_NONE, "No texture currently bound (nothing to unbind)");

    glBindTexture(GL_TEXTURE_2D, GL_NONE);
    fTexture = GL_NONE;
}

GLuint BoundTexture()
{
    assert(fTexture == QueryTexture());
    return fTexture;
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
    AddName(cubemap->id, path[0]);
}

void DestroyCubemap(Cubemap* cubemap)
{
    assert(cubemap->id != QueryCubemap(), "Can't delete a bound cubemap");
    assert(cubemap->id != GL_NONE, "Invalid cubemap handle");

    RemoveName(cubemap->id);
    glDeleteTextures(1, &cubemap->id);
    cubemap->id = GL_NONE;
}

void BindCubemap(Cubemap cubemap)
{
    assert(cubemap.id != GL_NONE, "Can't bind null cubemap");
    assert(cubemap.id != QueryCubemap(), "Cubemap already bound");

    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap.id);
    fCubemap = fCubemap;
}

void UnbindCubemap(Cubemap cubemap)
{
    assert(cubemap.id != GL_NONE);
    assert(QueryCubemap() != GL_NONE, "No cubemap currently bound (nothing to unbind)");

    glBindTexture(GL_TEXTURE_CUBE_MAP, GL_NONE);
    fCubemap = GL_NONE;
}

GLuint BoundCubemap()
{
    return fCubemap;
}

void CreateTextures()
{
}

void DestroyTextures()
{
}
