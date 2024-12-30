// Probably don't need this API yet, but separating loading files vs uploading to GPU makes things easier.
// Probably better to switch to this sooner than later before the Texture class is used in 100+ places.
#pragma once
#include <glad/glad.h>

struct Texture1D
{
    GLuint id = GL_NONE;
    int unit = -1;
};

struct Texture2D
{
    GLuint id = GL_NONE;
    int unit = -1;
};

struct Texture3D
{
    GLuint id = GL_NONE;
    int unit = -1;
};

struct TextureCubemap
{
    GLuint id = GL_NONE;
    int unit = -1;
};

void CreateTexture1D(Texture1D* texture, int width, int internalFormat, int format, int type, int filter, void* pixels = nullptr);
void CreateTexture2D(Texture2D* texture, int width, int height, int internalFormat, int format, int type, int filter, void* pixels = nullptr);
void CreateTexture3D(Texture3D* texture, int width, int height, int depth, int internalFormat, int format, int type, int filter, void* pixels = nullptr);
void CreateTextureCubemap(TextureCubemap* texture, int width, int height, int internalFormat, int format, int type, int filter, void* pixels[6] = nullptr);

void DestroyTexture1D(Texture1D* texture);
void DestroyTexture2D(Texture2D* texture);
void DestroyTexture3D(Texture3D* texture);
void DestroyTextureCubemap(TextureCubemap* texture);

void BindTexture1D(Texture1D* texture, GLuint unit);
void BindTexture2D(Texture2D* texture, GLuint unit);
void BindTexture3D(Texture3D* texture, GLuint unit);
void BindTextureCubemap(TextureCubemap* texture, GLuint unit);

void UnbindTexture1D(Texture1D* texture);
void UnbindTexture2D(Texture2D* texture);
void UnbindTexture3D(Texture3D* texture);
void UnbindTextureCubemap(TextureCubemap* texture);

// New API internals are looking pretty based TBH (super-safe validation wrapped in pretty little assertions)!