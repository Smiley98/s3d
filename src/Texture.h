#pragma once
#include <glad/glad.h>

struct Texture
{
    GLuint id = GL_NONE;
    int width = -1;
    int height = -1;

    int format = -1;
    int type = -1;
    int filter = -1;
};

void CreateTextureFromFile(Texture* texture, const char* path, bool flip = false);
void CreateTextureFromMemory(Texture* texture, int width, int height, unsigned char* pixels = nullptr/*RGBA, 32-bits per pixel*/);
void CreateTextureFromMemoryEx(Texture* texture, int width, int height, int internalFormat, int format, int type, int filter, unsigned char* pixels = nullptr);

void UpdateTexture(Texture texture, unsigned char* pixels);
void DestroyTexture(Texture* texture);

void BindTexture(Texture texture, GLuint slot = 0);
void UnbindTexture(Texture texture);

GLuint BoundTexture();

struct Cubemap
{
    GLuint id = GL_NONE;
};

void CreateCubemap(Cubemap* cubemap, const char* path[6]);
void DestroyCubemap(Cubemap* cubemap);

void BindCubemap(Cubemap cubemap);
void UnbindCubemap(Cubemap cubemap);

GLuint BoundCubemap();

extern Texture gTexHead;
extern Cubemap gSkybox;

void CreateTextures();
void DestroyTextures();
