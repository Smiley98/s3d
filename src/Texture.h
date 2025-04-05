#pragma once
#include <glad/glad.h>

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

struct TextureCubemap
{
    GLuint id = GL_NONE;
};

void CreateTexture1D(Texture1D* texture, int width, int internalFormat, int format, int type, int filter, void* pixels = nullptr);
void CreateTexture2D(Texture2D* texture, int width, int height, int internalFormat, int format, int type, int filter, void* pixels = nullptr);
void CreateTexture3D(Texture3D* texture, int width, int height, int depth, int internalFormat, int format, int type, int filter, void* pixels = nullptr);
void CreateTextureCubemap(TextureCubemap* cubemap, int width, int height, int internalFormat, int format, int type, int filter, void* pixels[6] = nullptr);

void DestroyTexture1D(Texture1D* texture);
void DestroyTexture2D(Texture2D* texture);
void DestroyTexture3D(Texture3D* texture);
void DestroyTextureCubemap(TextureCubemap* cubemap);

void BindTexture1D(Texture1D texture, GLuint unit);
void BindTexture2D(Texture2D texture, GLuint unit);
void BindTexture3D(Texture3D texture, GLuint unit);
void BindTextureCubemap(TextureCubemap cubemap, GLuint unit);

void UnbindTexture1D(Texture1D texture, GLuint unit);
void UnbindTexture2D(Texture2D texture, GLuint unit);
void UnbindTexture3D(Texture3D texture, GLuint unit);
void UnbindTextureCubemap(TextureCubemap cubemap, GLuint unit);

// Easier to add texture units everywhere and just assign them on the fly for now.
//constexpr GLuint TEXTURE_UNIT_DEFAULT = 0;
//constexpr GLuint TEXTURE_UNIT_FBO_POSITIONS = 1;
//constexpr GLuint TEXTURE_UNIT_FBO_NORMALS = 2;
//constexpr GLuint TEXTURE_UNIT_FBO_ALBEDO = 3;
//constexpr GLuint TEXTURE_UNIT_CUBEMAP = 4;
