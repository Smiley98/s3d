// Probably don't need this API yet, but separating loading files vs uploading to GPU makes things easier.
// Probably better to switch to this sooner than later before the Texture class is used in 100+ places.
#pragma once
#include <glad/glad.h>

// Probably don't need these yet. Easier to add texture units everywhere and just assign them on the fly for now.
// (I don't normally use more than 1 unit for 90% of draw calls, and deferred needs 3. Maybe a cubemap in there somewhere).
//constexpr GLuint TEXTURE_UNIT_DEFAULT = 0;
//constexpr GLuint TEXTURE_UNIT_FBO_POSITIONS = 1;
//constexpr GLuint TEXTURE_UNIT_FBO_NORMALS = 2;
//constexpr GLuint TEXTURE_UNIT_FBO_ALBEDO = 3;
//constexpr GLuint TEXTURE_UNIT_CUBEMAP = 4;

struct Texture1D
{
    GLuint id = GL_NONE;
};

struct Texture2D
{
    GLuint id = GL_NONE;
};

struct Texture3D
{
    GLuint id = GL_NONE;
};

struct Cubemap
{
    GLuint id = GL_NONE;
};

void CreateTexture1D(Texture1D* texture, int width, int internalFormat, int format, int type, int filter, void* pixels = nullptr);
void CreateTexture2D(Texture2D* texture, int width, int height, int internalFormat, int format, int type, int filter, void* pixels = nullptr);
void CreateTexture3D(Texture3D* texture, int width, int height, int depth, int internalFormat, int format, int type, int filter, void* pixels = nullptr);
void CreateCubemap(Cubemap* cubemap, int width, int height, int internalFormat, int format, int type, int filter, void* pixels[6] = nullptr);

void DestroyTexture1D(Texture1D* texture);
void DestroyTexture2D(Texture2D* texture);
void DestroyTexture3D(Texture3D* texture);
void DestroyCubemap(Cubemap* cubemap);

void BindTexture1D(Texture1D texture, GLuint unit);
void BindTexture2D(Texture2D texture, GLuint unit);
void BindTexture3D(Texture3D texture, GLuint unit);
void BindCubemap(Cubemap cubemap, GLuint unit);

void UnbindTexture1D(Texture1D texture, GLuint unit);
void UnbindTexture2D(Texture2D texture, GLuint unit);
void UnbindTexture3D(Texture3D texture, GLuint unit);
void UnbindCubemap(Cubemap cubemap, GLuint unit);

// New API internals are looking pretty based TBH (super-safe validation wrapped in pretty little assertions)!