#pragma once
#include <glad/glad.h>
#include <cassert>

struct Image;
struct Texture
{
    GLuint id = GL_NONE;
    int width = 0;
    int height = 0;
};

void CreateTexture(Texture* texture, int width, int height);
void DestroyTexture(Texture* texture);

void CreateTextureFromFile(Texture* texture, const char* path, bool flip = false);
void CreateTextureFromImage(Texture* texture, const Image& image);
void UpdateTexture(Texture texture, const Image& image);

inline void BindTexture(Texture texture, GLuint slot = 0)
{
    assert(texture.id != GL_NONE);
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, texture.id);
}

inline void UnbindTexture(Texture texture)
{
    assert(texture.id != GL_NONE);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
}

// Just a type-safe wrapper for a GL handle (consider moving bind & unbind for texture & cubemap to Pipeline.h)
struct Cubemap
{
    GLuint id = GL_NONE;
};

void CreateCubemap(Cubemap* cubemap, const char* path[6]);
void DestroyCubemap(Cubemap* cubemap);

inline void BindCubemap(Cubemap cubemap)
{
    assert(cubemap.id != GL_NONE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap.id);
}

inline void UnbindCubemap(Cubemap cubemap)
{
    assert(cubemap.id != GL_NONE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, GL_NONE);
}
