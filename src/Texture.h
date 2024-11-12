#pragma once
#include <glad/glad.h>

struct Texture
{
    GLuint id = GL_NONE;
    int width = 0;
    int height = 0;
};

void CreateTextureFromFile(Texture* texture, const char* path, bool flip = false);
void CreateTextureFromMemory(Texture* texture, int width, int height, unsigned char* pixels = nullptr/*RGBA, 32-bits per pixel*/);

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
