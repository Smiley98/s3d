#pragma once
#include <glad/glad.h>

void CreateTextures();
void DestroyTextures();

struct Texture
{
    GLuint id = GL_NONE;
};

void CreateTextureFromMemory(Texture* texture, int width, int height, int internalFormat, int format, int type, int filter, unsigned char* pixels = nullptr);
void CreateTextureFromFile(Texture* texture, const char* path, bool flip = false);
void DestroyTexture(Texture* texture);

void BindTexture(Texture texture, GLuint slot);
void UnbindTexture(Texture texture, GLuint slot);

struct Cubemap
{
    GLuint id = GL_NONE;
};

void CreateCubemap(Cubemap* cubemap, const char* path[6]);
void DestroyCubemap(Cubemap* cubemap);

void BindCubemap(Cubemap cubemap);
void UnbindCubemap(Cubemap cubemap);
